/**
 * Name: Snowy Chen, Joe Song
 * Date: 6 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "linus_app.h"

const char *prod_signal_msg = "done";

Set::Set(DataFrame *df) : Set(df->nrows()) {}

Set::Set(size_t sz) : vals(new bool[sz]), num_elements(sz) {
  for (size_t i = 0; i < num_elements; i++)
    vals[i] = false;
}

Set::~Set() { delete[] vals; }

void Set::set(size_t idx) {
  if (idx >= num_elements)
    return; // ignoring out of bound writes
  vals[idx] = true;
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

ProjectsTagger::ProjectsTagger(Set &uSet, Set &pSet, size_t num_proj)
    : uSet(uSet), pSet(pSet), newProjects(num_proj) {}

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

UsersTagger::UsersTagger(Set &pSet, Set &uSet, size_t num_proj)
    : pSet(pSet), uSet(uSet), newUsers(num_proj) {}

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

LinusStatusHandler::LinusStatusHandler(Lock *distro_complete_signal) {
  assert(distro_complete_signal);
  this->signal = distro_complete_signal;
}

bool LinusStatusHandler::handle_status(Status *msg) {
  String *message = msg->get_message();
  String expected_msg(prod_signal_msg);

  if (expected_msg.equals(message)) {
    // We got the expected message stating that the distribution of the
    // dataframe has been completed. Flip on the signal to start the local
    // counting
    this->signal->notify_all();
  }

  delete message;
  return false;
}

/****************************************************************************/

Linus::Linus() : Application(Linus::NUM_NODES) {
  this->uSet = nullptr;
  this->pSet = nullptr;

  this->status_handler = new LinusStatusHandler(&this->distribution_signal);
  this->proj_num = 0;
  this->user_num = 0;
  this->commit_num = 0;
}

Linus::~Linus() {
  delete this->status_handler;
  delete this->uSet;
  delete this->pSet;
}

void Linus::main() {
  readInput();
  for (size_t i = 0; i < DEGREES; i++)
    step(i);
}

/** Node 0 reads three files, cointainng projects, users and commits, and
 *  creates thre dataframes. All other nodes wait and load the three
 *  dataframes. Once we know the size of users and projects, we create
 *  sets of each (uSet and pSet). We also output a data frame with a the
 *  'tagged' users. At this point the dataframe consists of only
 *  Linus. **/
void Linus::readInput() {
  if (this->get_node_id() == 0) {
    printf("Reading ");
    this->proj_num = KeyValueStore::from_file(this->proj_key, this->kv,
        this->PROJECT_FILENAME);
    printf("%zu projects...\n", this->proj_num);


    printf("Reading ");
    this->user_num = KeyValueStore::from_file(this->user_key, this->kv,
        this->USER_FILENAME);
    printf("%zu users...\n", this->user_num);

    printf("Reading ");
    this->commit_num = KeyValueStore::from_file(this->commit_key, this->kv,
        this->COMMIT_FILENAME);
    printf("%zu commits...\n", this->commit_num);

    // This dataframe contains the id of Linus.
    KeyValueStore::from_scalar(this->linus_id_key, this->kv, Linus::LINUS);
    KeyValueStore::from_scalar(this->num_proj_key, this->kv,
        static_cast<int>(this->proj_num));
    KeyValueStore::from_scalar(this->num_users_key, this->kv,
                               static_cast<int>(this->user_num));
    KeyValueStore::from_scalar(this->num_commit_key, this->kv,
                               static_cast<int>(this->commit_num));

    // Now notify that the dataframe has finished being produced.
    String production_done_msg(prod_signal_msg);
    for (size_t i = 1; i < NUM_NODES; i++) {
      this->kv->send_status_message(i, production_done_msg);
    }
  } else {
    // Wait for the production of the dataframes to complete
    this->distribution_signal.wait();

    DataFrame *df_proj = this->kv->wait_and_get(this->num_proj_key);
    DataFrame *df_user = this->kv->wait_and_get(this->num_users_key);
    DataFrame *df_commit = this->kv->wait_and_get(this->num_commit_key);
    this->proj_num = df_proj->get_int(0, 0);
    this->user_num = df_user->get_int(0, 0);
    this->commit_num = df_commit->get_int(0, 0);
    delete df_proj;
    delete df_user;
    delete df_commit;
  }

  this->uSet = new Set(this->user_num);
  this->pSet = new Set(this->proj_num);
}

void Linus::step(int stage) {
  printf("Stage %d\n", stage);
  // Key of the shape: users-stage-0
  String *uk_name = StrBuff().c("users-").c(stage).c("-0").get();
  Key uK(uk_name->c_str(), 0);

  // A df with all the users added on the previous round
  DataFrame *newUsers = this->kv->wait_and_get(uK);
  Set delta(this->user_num);
  SetUpdater upd(delta);
  newUsers->map(upd); // all of the new users are copied to delta.
  delete newUsers;

  ProjectsTagger ptagger(delta, *pSet, this->proj_num);
  // marking all projects touched by delta
  this->kv->local_map(this->commit_key, ptagger);
  this->merge(ptagger.newProjects, "projects-", stage);
  pSet->union_set(ptagger.newProjects);

  UsersTagger utagger(ptagger.newProjects, *uSet, this->user_num);
  this->kv->local_map(this->commit_key, utagger);
  merge(utagger.newUsers, "users-", stage + 1);
  uSet->union_set(utagger.newUsers);

  printf("    after stage %i:\n", stage);
  printf("        tagged projects: %zu\n", this->pSet->size());
  printf("        tagged users: %zu\n", this->uSet->size());

  delete uk_name;
}

void Linus::merge(Set &set, char const *name, int stage) {
  if (this->get_node_id() == 0) {
    for (size_t i = 1; i < this->kv->get_num_nodes(); ++i) {
      String *key_name = StrBuff().c(name).c(stage).c("-").c(i).get();
      Key nK(key_name->c_str(), 0);
      delete key_name;

      DataFrame *delta = kv->wait_and_get(nK);
      printf("    received delta of %zu elements from node %zu\n",
          delta->nrows(), i);
      SetUpdater upd(set);
      delta->map(upd);
      delete delta;
    }

    printf("    storing %zu merged elements\n", set.size());
    SetWriter writer(set);
    String *key_name = StrBuff().c(name).c(stage).c("-0").get();
    Key k(key_name->c_str(), 0);
    delete key_name;
    KeyValueStore::from_visitor(k, this->kv, "I", writer);
  } else {
    printf("    sending %zu elements to master node\n", set.size());

    SetWriter writer(set);
    String *key_name = StrBuff().c(name).c(stage).c("-").c(this->get_node_id()).get();
    Key k(key_name->c_str(), 0);
    delete key_name;
    KeyValueStore::from_visitor(k, this->kv, "I", writer);

    key_name = StrBuff().c(name).c(stage).c("-0").get();
    Key mK(key_name->c_str(), 0);
    delete key_name;
    DataFrame *merged = this->kv->wait_and_get(mK);

    printf("    receiving %zu merged elements\n", merged->nrows());
    SetUpdater upd(set);
    merged->map(upd);
    delete merged;
  }
}
