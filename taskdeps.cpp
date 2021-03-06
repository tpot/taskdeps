/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/set.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class TaskDeps : public Script {

private:

  static const int max_tasks = 6;
  SetVarArray deps;

public:

  TaskDeps(const Options& opt)
    : Script(opt), deps(*this, max_tasks, IntSet::empty, IntSet(1, 6))
  {
    // Sets must be disjoint between each other
    // TODO: replace with count() constraint

    for (int i = 0; i < max_tasks; i++) {
      for (int j = 0; j < max_tasks; j++) {
        if (i != j) {
          rel(*this, deps[i], SRT_DISJ, deps[j]);
        }
      }
    }

    // Only do one at a time

    for (int i = 0; i < max_tasks; i++) {
      cardinality(*this, deps[i], 0, 2);
    }

    // Task ordering constraints

    precede(*this, deps, 1, 2); // [1] -> [2]

    precede(*this, deps, 2, 5); // [2, 3, 4] -> [5]
    precede(*this, deps, 3, 5);
    precede(*this, deps, 4, 5);

    precede(*this, deps, 1, 6); // [1, 5] -> [6]
    precede(*this, deps, 5, 6);

    // We want to build target 6 so it must appear somewhere

    rel(*this, setunion(deps) >= IntSet(IntArgs(1, 6)));

    // Null tasks all towards the end

    for (int i = 0; i < max_tasks - 1; i++) {
      BoolVar currIsEmpty = expr(*this, (deps[i] == IntSet::empty));
      BoolVar nextIsEmpty = expr(*this, (deps[i + 1] == IntSet::empty));

      rel(*this, currIsEmpty >> nextIsEmpty);
    }

    // Brancher

    branch(*this, deps, SET_VAR_NONE(), SET_VAL_MIN_INC());
  }

  TaskDeps(bool share, TaskDeps& s) : Script(share,s) 
  {
    deps.update(*this, share, s.deps);
  }

  virtual Space* copy(bool share) 
  {
    return new TaskDeps(share,*this);
  }

  virtual void print(std::ostream& os) const {
    os << "\t" << deps << std::endl;
  }

};

int main(int argc, char* argv[]) 
{
  Options opt("grid");
  opt.solutions(0);
  opt.iterations(1500);
  opt.parse(argc,argv);
  Script::run<TaskDeps,DFS,Options>(opt);
  return 0;
}
