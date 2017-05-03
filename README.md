# About
This application allows you to bring to front few important windows with a hotkey instead of Alt+Tab'ing through tons of openend windows. 

# Config

## Example of the config

    # trigger combination
    hotkey = Shift+Ctrl+0

    # sorted descending by priority
    app = qt creator
    app = visual studio
    app = emacs
    app = sublime
    app = atom
    app = notepad++
    app = code

    # activate minimum 2 windows (e.g. if you have 2 monitors)
    min_windows = 2
    
## Explanation

### app
You enumerate parts of the window names you want to restore in descending order. In the example above windows _"qt creator"_ and _"visual studio"_ will be restored before _"emacs"_ or _"sublime"_ if they will be found open.

### hotkey
Specifies a hotkey for bringing windows to front. You can choose combinations of `shift\ctrl\alt` and `A-Z`, `0-9`.

### min_windows
If you have 2 monitors you might want to restore a window per monitor therefore you set `min_windows` variable to `2`.

# How to contribute:
- [Fork](http://help.github.com/forking/) BackToWork repository on GitHub
- Clone your fork locally
- Configure the upstream repo (`git remote add upstream git@github.com:Ribtoks/BackToWork.git`)
- Create local branch (`git checkout -b your_feature`)
- Work on your feature
- Push the branch to GitHub (`git push origin your_feature`)
- Send a [pull request](https://help.github.com/articles/using-pull-requests) on GitHub
