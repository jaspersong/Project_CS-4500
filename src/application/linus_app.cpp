/**
 * Name: Snowy Chen, Joe Song
 * Date: 6 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "linus_app.h"

Set::Set(DistributedValue *df) : Set(df->nrows()) {}

Set::Set(size_t sz) : vals(new bool[sz]), num_elements(sz) {
  for (size_t i = 0; i < num_elements; i++)
    vals[i] = false;
  this->num_set = 0;
}

Set::~Set() { delete[] vals; }

void Set::set(size_t idx) {
  if (idx >= num_elements) {
    return; // ignoring out of bound writes
  }

  if (!vals[idx]) {
    vals[idx] = true;
    this->num_set += 1;
  }
}

bool Set::test(size_t idx) {
  if (idx >= num_elements)
    return true; // ignoring out of bound reads
  return vals[idx];
}

size_t Set::size() { return num_elements; }

void Set::union_set(Set &from) {
  for (size_t i = 0; i < from.num_elements; i++)
    if (from.test(i))
      set(i);
}

size_t Set::size_set() { return this->num_set; }

/****************************************************************************/

SetUpdater::SetUpdater(Set &set) : set(set) {}

bool SetUpdater::accept(Row &row) {
  set.set(row.get_int(0));
  return false;
}

/****************************************************************************/

SetWriter::SetWriter(Set &set) : set(set) {}

/** Skip over false values and stop when the entire set has been seen */
bool SetWriter::done() {
  while ((pos < set.size()) && !set.test(pos)) {
    ++pos;
  }
  return pos == set.size();
}

void SetWriter::visit(Row &row) { row.set(0, pos++); }

/****************************************************************************/

ProjectsTagger::ProjectsTagger(Set& uSet, Set& pSet, DistributedValue* proj)
    : uSet(uSet), pSet(pSet), newProjects(proj) {}

bool ProjectsTagger::accept(Row &row) {
  int pid = row.get_int(0);
  int uid = row.get_int(1);
  if (uSet.test(uid))
    if (!pSet.test(pid)) {
      pSet.set(pid);
      newProjects.set(pid);
    }
  return false;
}

/****************************************************************************/

UsersTagger::UsersTagger(Set& pSet,Set& uSet, DistributedValue* users)
    : pSet(pSet), uSet(uSet), newUsers(users) {}

bool UsersTagger::accept(Row &row) {
  int pid = row.get_int(0);
  int uid = row.get_int(1);
  if (pSet.test(pid))
    if (!uSet.test(uid)) {
      uSet.set(uid);
      newUsers.set(uid);
    }
  return false;
}

/****************************************************************************/

Linus::Linus() : Application(Linus::NUM_NODES) {
  this->uSet = nullptr;
  this->pSet = nullptr;

  this->projects = nullptr;
  this->users = nullptr;
  this->commits = nullptr;
}

Linus::~Linus() {
  delete this->uSet;
  delete this->pSet;
}

void Linus::main() {
  readInput();
  for (size_t i = 0; i < DEGREES; i++) {
    step(i);
  }
}

/** Node 0 reads three files, cointainng projects, users and commits, and
 *  creates thre dataframes. All other nodes wait and load the three
 *  dataframes. Once we know the size of users and projects, we create
 *  sets of each (uSet and pSet). We also output a data frame with a the
 *  'tagged' users. At this point the dataframe consists of only
 *  Linus. **/
void Linus::readInput() {
  Key pK("projs");
  Key uK("usrs");
  Key cK("comts");

  if (this->get_node_id() == 0) {
    printf("Reading ");
    KeyValueStore::from_file(pK, this->kv, this->PROJECT_FILENAME);
    this->projects = this->kv->wait_and_get(pK);
    printf("%zu projects...\n", this->projects->nrows());

    printf("Reading ");
    KeyValueStore::from_file(uK, this->kv, this->USER_FILENAME);
    this->users = this->kv->wait_and_get(uK);
    printf("%zu users...\n", this->users->nrows());

    printf("Reading ");
    KeyValueStore::from_file(cK, this->kv, this->COMMIT_FILENAME);
    this->commits = this->kv->wait_and_get(cK);
    printf("%zu commits...\n", this->commits->nrows());

    // This dataframe contains the id of Linus.
    Key linus_id("users-0-0");
    KeyValueStore::from_scalar(linus_id, this->kv, Linus::LINUS);
  } else {
    this->projects = this->kv->wait_and_get(pK);
    this->users = this->kv->wait_and_get(uK);
    this->commits = this->kv->wait_and_get(cK);
  }

  this-> uSet = new Set(this->users);
  this-> pSet = new Set(this->projects);
}

void Linus::step(int stage) {
  printf("Stage %d\n", stage);
  // Key of the shape: users-stage-0
  String *uk_name = StrBuff().c("users-").c(stage).c("-0").get();
  Key uK(uk_name->c_str());
  delete uk_name;

  // A df with all the users added on the previous round
  DistributedValue *new_users = this->kv->wait_and_get(uK);
  Set delta(this->users);
  SetUpdater upd(delta);
  new_users->map(upd); // all of the new users are copied to delta.

  ProjectsTagger ptagger(delta, *this->pSet, this->projects);
  this->commits->local_map(ptagger); // marking all projects touched by delta
  this->merge(ptagger.newProjects, "projects-", stage);
  pSet->union_set(ptagger.newProjects);

  UsersTagger utagger(ptagger.newProjects, *this->uSet, this->users);
  this->commits->local_map(utagger);
  this->merge(utagger.newUsers, "users-", stage + 1);
  uSet->union_set(utagger.newUsers);

  printf("    %zu: after stage %i:\n", this->get_node_id(), stage);
  printf("        %zu: tagged projects: %zu\n",
      this->get_node_id(), this->pSet->size_set());
  printf("        %zu: tagged users: %zu\n",
      this->get_node_id(), this->uSet->size_set());
}

void Linus::merge(Set &set, char const *name, int stage) {
  if (this->get_node_id() == 0) {
    for (size_t i = 1; i < this->kv->get_num_nodes(); ++i) {
      printf("    %zu: %s master node has %zu elements\n",
             this->get_node_id(), name, set.size_set());

      String *key_name = StrBuff().c(name).c(stage).c("-").c(i).get();
      Key nK(key_name->c_str());
      delete key_name;

      DistributedValue *delta = kv->wait_and_get(nK);
      printf("    %zu: %s received delta of %zu elements from node %zu\n",
          this->get_node_id(), name, delta->nrows(), i);
      SetUpdater upd(set);
      delta->map(upd);
    }

    printf("    %zu: %s storing %zu merged elements\n", this->get_node_id(),
        name, set.size_set());
    SetWriter writer(set);
    String *key_name = StrBuff().c(name).c(stage).c("-0").get();
    Key k(key_name->c_str());
    delete key_name;
    KeyValueStore::from_visitor(k, this->kv, "I", writer);
  } else {
    printf("    %zu: %s sending %zu elements to master node\n",
        this->get_node_id(), name, set.size_set());

    SetWriter writer(set);
    String *key_name = StrBuff().c(name).c(stage).c("-").c(this->get_node_id()).get();
    Key k(key_name->c_str());
    delete key_name;
    KeyValueStore::from_visitor(k, this->kv, "I", writer);

    key_name = StrBuff().c(name).c(stage).c("-0").get();
    Key mK(key_name->c_str());
    delete key_name;
    DistributedValue *merged = this->kv->wait_and_get(mK);

    printf("    %zu: %s receiving %zu merged elements\n",
        this->get_node_id(), name, merged->nrows());
    SetUpdater upd(set);
    merged->map(upd);
  }
}
