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
