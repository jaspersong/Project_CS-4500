/**
 * Name: Snowy Chen, Joe Song
 * Date: 6 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "application.h"
#include "key.h"
#include "application_network_interface.h"

/**
 * The input data is a processed extract from GitHub.
 *
 * projects:  I x S   --  The first field is a project id (or pid).
 *                    --  The second field is that project's name.
 *                    --  In a well-formed dataset the largest pid
 *                    --  is equal to the number of projects.
 *
 * users:    I x S    -- The first field is a user id, (or uid).
 *                    -- The second field is that user's name.
 *
 * commits: I x I x I -- The fields are pid, uid, uid', each row represent
 *                    -- a commit to project pid, written by user uid
 *                    -- and committed by user uid',
 **/

/**************************************************************************
 * A bit set contains size() booleans that are initialize to false and can
 * be set to true with the set() method. The test() method returns the
 * value. Does not grow.
 ************************************************************************/
class Set {
public:
  /** Creates a set of the same size as the dataframe. */
  explicit Set(DistributedValue* df);

  /** Creates a set of the given size. */
  explicit Set(size_t sz);

  ~Set();

  /** Add idx to the set. If idx is out of bound, ignore it.  Out of bound
   *  values can occur if there are references to pids or uids in commits
   *  that did not appear in projects or users.
   */
  void set(size_t idx);

  /** Is idx in the set?  See comment for set(). */
  bool test(size_t idx);

  /** Performs set union in place. */
  void union_set(Set& from);

  size_t size();
  size_t size_set();

private:
  bool*vals;  // owned; data
  size_t num_elements; // number of elements
  size_t num_set; // Number of elements that were set
};

/*******************************************************************************
 * A SetUpdater is a reader that gets the first column of the data frame and
 * sets the corresponding value in the given set.
 ******************************************************************************/
class SetUpdater : public Reader {
public:
  explicit SetUpdater(Set& set);

  /** Assume a row with at least one column of type I. Assumes that there
   * are no missing. Reads the value and sets the corresponding position.
   * The return value is irrelevant here. */
  bool accept(Row & row) override;

private:
  Set& set; // set to update
};

/*****************************************************************************
 * A SetWriter copies all the values present in the set into a one-column
 * dataframe. The data contains all the values in the set. The dataframe has
 * at least one integer column.
 ****************************************************************************/
class SetWriter: public Writer {
public:
  explicit SetWriter(Set& set);

  /** Skip over false values and stop when the entire set has been seen */
  bool done() override;

  void visit(Row & row) override;

private:
  Set& set; // set to read from
  int pos = 0;  // position in set
};

/***************************************************************************
 * The ProjectTagger is a reader that is mapped over commits, and marks all
 * of the projects to which a collaborator of Linus committed as an author.
 * The commit dataframe has the form:
 *    pid x uid x uid
 * where the pid is the identifier of a project and the uids are the
 * identifiers of the author and committer. If the author is a collaborator
 * of Linus, then the project is added to the set. If the project was
 * already tagged then it is not added to the set of newProjects.
 *************************************************************************/
class ProjectsTagger : public Reader {
public:
  Set newProjects;  // newly tagged collaborator projects

  ProjectsTagger(Set& uSet, Set& pSet, DistributedValue* proj);

  /** The data frame must have at least two integer columns. The newProject
   * set keeps track of projects that were newly tagged (they will have to
   * be communicated to other nodes). */
  bool accept(Row & row) override;

private:
  Set& uSet; // set of collaborator
  Set& pSet; // set of projects of collaborators
};

/***************************************************************************
 * The UserTagger is a reader that is mapped over commits, and marks all of
 * the users which commmitted to a project to which a collaborator of Linus
 * also committed as an author. The commit dataframe has the form:
 *    pid x uid x uid
 * where the pid is the idefntifier of a project and the uids are the
 * identifiers of the author and committer.
 *************************************************************************/
class UsersTagger : public Reader {
public:
  Set newUsers;

  UsersTagger(Set& pSet,Set& uSet, DistributedValue* users);

  bool accept(Row & row) override;

private:
  Set& pSet;
  Set& uSet;
};

/*************************************************************************
 * This computes the collaborators of Linus Torvalds.
 * is the linus example using the adapter.  And slightly revised
 *   algorithm that only ever trades the deltas.
 **************************************************************************/
class Linus : public Application {
public:
  // TODO: 8 and above nodes causes problems with sending directory messages
  //  from the server
  static const size_t NUM_NODES = 3;

  int DEGREES = 4;  // How many degrees of separation form linus?
  int LINUS = 4967;   // The uid of Linus (offset in the user df)
  const char* PROJECT_FILENAME = "../data/projects.ltgt";
  const char* USER_FILENAME = "../data/users.ltgt";
  const char* COMMIT_FILENAME = "../data/commits.ltgt";

  Linus();
  ~Linus() override;

  /** Compute DEGREES of Linus.  */
  void main() override;

  /** Node 0 reads three files, cointainng projects, users and commits, and
   *  creates thre dataframes. All other nodes wait and load the three
   *  dataframes. Once we know the size of users and projects, we create
   *  sets of each (uSet and pSet). We also output a data frame with a the
   *  'tagged' users. At this point the dataframe consists of only
   *  Linus. **/
  void readInput();

  /** Performs a step of the linus calculation. It operates over the three
   *  datafrrames (projects, users, commits), the sets of tagged users and
   *  projects, and the users added in the previous round. */
  void step(int stage);

  /** Gather updates to the given set from all the nodes in the systems.
   * The union of those updates is then published as dataframe.  The key
   * used for the otuput is of the form "name-stage-0" where name is either
   * 'users' or 'projects', stage is the degree of separation being
   * computed.
   */
  void merge(Set& set, char const* name, int stage);

private:
  DistributedValue* projects; //  pid x project name
  DistributedValue* users;  // uid x user name
  DistributedValue* commits;  // pid x uid x uid

  Set* uSet; // Linus' collaborators
  Set* pSet; // projects of collaborators
};
