# Command Line

A library to parse the command line.

## xcmdline, cmdline library

A hand-written cmdline arguments parser using xva-list from xbase library.
Note: Currently can only handle command-line in ASCII

## Example

```c++
s32 prop_month = 0;
s32 prop_day = 0;
s32 prop_year = 0;
xcore::cli::argV argv[] = {
    xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_month)),
    xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_day)),
    xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_year)),
    xcore::cli::argV::nil
};

cmdline c;
c.parse(argv, "--month 8 --day 12 -y 2020");
```

## Command based

We want another indirection which is identical to git, e.g.:

* git add
* git commit
* git push

Introducing commands where every command has a list of arguments:

```c++
const char* prop_commit_msg = nullptr;
xcore::cli::cmd cmds[] = {
    {
        "add",
        {
            xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_month)),
            xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_day)),
            xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_year)),
            xcore::cli::argV::nil
        }
    },
    {
        "commit",
        {
            xcore::cli::argV("m", "message", "Message", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_commit_msg)),
            xcore::cli::argV::nil
        }
    }
};

cmdline c;
xcore::cli::cmd* cmd = c.parse(cmds, "commit -m `this is our first commit`");

```

Commands are searched, so this means that if you only have the 2 above commands you can differentiate between them
just by a 'a' and a 'c', so this would work:

* git a
* git c
