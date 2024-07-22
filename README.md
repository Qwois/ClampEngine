![](http://github-profile-summary-cards.vercel.app/api/cards/most-commit-language?username=vn7n24fzkq&theme=default)

# Branches

We publish the source code for the engine across several branches:

**Numbered branches** identify past and upcoming official releases. The release branch always reflects the current official release. These branches are extensively tested by our QA team, making them an excellent starting point for learning ClampEngine and for creating your own games. We work hard to ensure that releases are stable and reliable, aiming to publish a new release every few months.

**The most active development** on ClampEngine happens in the `clamp-main` branch. This branch reflects the cutting edge of the engine and may be buggy — it might not even compile. We make it available for battle-hardened developers eager to test new features or work in lock-step with us.

If you choose to work in this branch, be aware that it is likely to be ahead of the branches for the current official release and the next upcoming release. Therefore, content and code created for the `clamp-main` branch may not be compatible with public releases until we create a new branch directly from `clamp-main` for a future official release.

**The master branch** serves as the hub of changes to ClampEngine from all our engine development teams. It is not subjected to as much testing as release branches, so if you are using ClampEngine, this may not be the best choice for you.

Branches with names containing `dev`, `staging`, and `test` are typically for internal processes and are rarely useful for end users.

**Other short-lived branches** may appear from time to time as we stabilize new releases or hotfixes.


# Install and Building

Foobar is a Python library for dealing with word pluralization.

## Installation

Use the package manager [vcpkg](https://vcpkg.io/en/) to install raylib.

# Navigate to the vcpkg directory, where vcpkg.exe will be located

```cmd
.\vcpkg.exe install raylib:x64-windows
```
## Build

1. Open the project.
2. Make sure all dependencies are connected
3. Begin building the project

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
