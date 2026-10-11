Aegys Standerd C Code Styling Guide

This document describes the standerd C code styling practices with the goal of enforcing a clean uniform codestyle.
This includes guidlines for:

 - Variable, Function, Macro Naming;
 - Commenting;
 - structuring of code;


 1. Variable, Function, Macro Naming;

    General Naming Rules:

    - All file structures (libuarys, etc) must have a unique name and short prefix. This short Prefix allways comes first in a name and is followed by an Underscore '_'
      from the rest of the name, witch may include other prefixes.
      for example:
      The Header files for Qualety_Text_System has the unique prefix QTS for all its global definitions,
      so a function that is globaly defined by these files must be named QTS_<function_name>(...).

    - The use of Emojis or symbols that are not part of the standerd ASCII table is not permitted outside of quoted strings.

    - All names, comments, descriptions, ecetera must be in English.




    Macro Naming:

    In addition to the General Naming Rules the following rules must be followed when naming macros.

    All Macro names must be written in all caps.
    All Macro names must not include spaces, words must be seperated only by underscore '_'.
    All Macro names may seperate anny words, that are not intentionaly combined, with an underscore '_';



    Function Naming:

    In addition to the General Naming Rules the following rules must be followed when naming functions.

    The name of a function must be structured in the following way:

    PREFIX_PREFIX_CoreName_SUFFIX_SUFFIX

    A number of Prefixes followed by the functions core name followed by a number of Suffixes.
    Prefixes, suffixes and CoreName are always seperated from each other with one underscore.

    The First Prefix is always the one denoting the definition source (what libuary defines the function),
    witch is usualy, but does not have to be, a small number of capitol letters.
    Prefixes themselfes may not contain non alpha numerical symbols like spaces, underscores, etc.

    The CoreName of a function is the name of the function at its core.
    The CoreName may not include non alpha numerical symbols like spcaes,+,-,%,$.
    The CoreName of a function may be composed of several words. Seperation of words in the Core name is done by making the first letter of the word its capitol form.

    A suffix may only be used if the function exsists in multiple itterations and suffixes may only be used to denote the itteration of that function.
    Iteration Herachy is orderd from front to back with the suffixes in front being highest in the herachy.

    Example:
    we have a function defined in the Qualety_Text_System headers witch has the prefix Init (because it initializes something) and the Core name "My function" witch has five iterations and each itteration has five subiterations.
    Then the name of the function will be: (for function iteration 5 comp 4 (note that these suffixes are just as examples))

    QTS_Init_MyFunction_I5_C4



    Variable Naming:

    In addition to the General Naming Rules the following rules must be followed when naming Variables.

    The name of a function must be structured in the following way:

    PREFIX_PREFIX_CoreName_SUFFIX_SUFFIX

    A number of Prefixes followed by the functions core name followed by a number of Suffixes.
    Prefixes, suffixes and CoreName are always seperated from each other with one underscore.

    The First Prefix is always the one denoting the definition source (what libuary defines the function),
    witch is usualy, but does not have to be, a small number of capitol letters.
    Prefixes themselfes may not contain non alpha numerical symbols like spaces, underscores, etc.
    Pointers must always have the Prefix "ptr" to make them identifiable as pointers, unless the pointer referes to a combined object like a string
    in witch case the "ptr" prefix may be optional.

    The CoreName of a Variable is the name of the function at its core.
    The CoreName may not include non alpha numerical symbols like spcaes,+,-,%,$.
    The CoreName of a function may be composed of several words. Seperation of words in the Core name is done by making the first letter of the word its capitol form.
