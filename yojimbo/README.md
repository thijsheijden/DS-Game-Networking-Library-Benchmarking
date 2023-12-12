# Distributed Systems: Performance Analysis of Game Networking Libraries

## Git Workflow
When working on an issue, bugfix, feature etc, please follow these steps:
1. Branch from dev following the branch naming conventions
2. Work on the feature branch, committing regularly following the commit conventions
3. Once the feature has been completed, create a PR to dev following the PR conventions
4. Get at least one other team member to review the changes
5. Upon approval merge the changes

### Branch Naming Conventions
#### New Features
For new features or functionality, like for instance a new command line argument option.

`feature/{issue-number}-issue-description-in-few-words`

Examples:
- feature/123-cpu-time-metric-logging
- feature/456-bandwidth-constriction-argument

#### Feature Improvements
For improvements to existing features, like for instance improving the performance of an existing feature and improving error strings and logging.

`improvement/{issue-number}-improvement-description-in-few-words`

Examples:
- improvement/123-better-cpu-time-logging
- improvement/456-format-error-strings

#### Bug Fixes
For bug fixes, like for instance fixing a NULL pointer dereference caused by lacking input sanitation/checking. 

`fix/{issue-number}-fixed-problem-description-in-few-words`

Examples:
- fix/123-nill-pointer-deref-user-handler
- fix/456-wrong-response-data

### Commit Conventions
1. Keep the commit title under 72 characters
2. Use the imperative present tense, e.g. "Add button to request support help" instead of "Added button ..."
3. The commit body can contain more information on the changes made in the commit
4. A commit should be a small change, and should aim to go from one working state to another working state, try to avoid committing non-working states
5. Commit often, avoid having to roll back too far when things stop working!

### PR Conventions
1. Before creating the PR, if your branch is too far off the target (most likely dev), rebase your branch to incorporate the changes made on the target branch
2. Title the PR "{issue-number}: Short change description"
3. Add a bulleted list to the PR body with the largest changes the PR makes. All items should be in the imperative present tense
