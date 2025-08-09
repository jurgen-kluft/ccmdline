# Command Line

A library to parse the command line.

## ccmdline, a cmdline library

A hand-written cmdline arguments parser using va_list_t from cbase library.

Note: Currently can only handle command-line in ASCII/UTF-8 encoding.

## Example

```c++
s32 prop_month = 0;
s32 prop_day = 0;
s32 prop_year = 0;
ncore::cli::argv argv[] = {
    ncore::cli::argv("m", "month", "Month", ncore::cli::Required, va_t(&prop_month)),
    ncore::cli::argv("d", "day", "Day", ncore::cli::Required, va_t(&prop_day)),
    ncore::cli::argv("y", "year", "Year", ncore::cli::Required, va_t(&prop_year)),
    ncore::cli::argv::nil
};

ncore::cli::cmdline c;
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
ncore::cli::cmd cmds[] = {
    {
        "add",
        {
            ncore::cli::argv("m", "month", "Month", ncore::cli::Required, va_t(&prop_month)),
            ncore::cli::argv("d", "day", "Day", ncore::cli::Required, va_t(&prop_day)),
            ncore::cli::argv("y", "year", "Year", ncore::cli::Required, va_t(&prop_year)),
            ncore::cli::argv::nil
        }
    },
    {
        "commit",
        {
            ncore::cli::argv("m", "message", "Message", ncore::cli::Required, va_t(&prop_commit_msg)),
            ncore::cli::argv::nil
        }
    }
};

ncore::cli::cmdline c;
ncore::cli::cmd* cmd = c.parse(cmds, "commit -m `this is our first commit`");

```

Commands are searched, so this means that if you only have the 2 above commands you can differentiate between them
just by a 'a' and a 'c', so this would work:

* git a
* git c
