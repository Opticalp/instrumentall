# Branching policy
We use a simple branching policy. 

 * **master** is the stable branch. Since there is no stable release published now (Jan., 2017), this branch is pretty empty. 
 * **develop** is the main development branch. Changes are managed using pull requests. Merging is only possible if the tests (TravisCI and AppVeyor) pass. 
 * ***anyBranch*** created from *develop* is considered as a feature branch. Its name should describe it well. 
 
## Forks
 
 For each implementation of InstrumentAll that is not designed to be merged back (*specialization*), a new fork should be created. 
 
 You shall fill:
  * [fork_NAME](/fork_NAME) with the name of your fork (e.g. *myImplementation*)
  * [fork_VERSION](/fork_VERSION) with the version of the fork (e.g. *2.0-dev.24*) that will be used to show the full version at run time, aside the main InstrumentAll version. 
  * [fork_CHANGELOG](/fork_CHANGELOG) with the changelog specific to the fork. 
   
As usual, the core features should be first develop in the main *develop* branch and then merged into the forks. In case of accidentaly development in the fork, we can implement it back in the main *develop* branch by `rebase --onto` (before publication) or `cherry-pick` (less destructive if already published). 

## Production machines

InstrumentAll is designed to be deployed on production sites, based on specific implementations (*forks*, see above). To keep track of the current version deployed on each production machine, it is possible to create a branch for each machine, pined on the built commit. Conf files and scripts can be added to those branches in the [runtimeEnv](/runtimeEnv) directory. 