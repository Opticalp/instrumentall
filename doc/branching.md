# Branching policy
We use a simple branching policy. 

 * **master** is the stable branch. Since there is no stable release published now (Jan., 2017), this branch is pretty empty. 
 * **develop** is the main development branch. Changes are managed using pull requests. Merging is only possible if the tests (TravisCI and AppVeyor) pass. 
 * ***anyBranch*** created from *develop* is considered as a feature branch. Its name should describe it well.  