# my_export Function - Complete Case Analysis and Visualization

This document provides a comprehensive analysis of the `my_export` function in minishell, covering all possible cases and execution paths.

## 🎯 Function Overview

The `my_export` function handles the `export` command in bash, which allows setting and exporting environment variables. It handles two main scenarios: variables with values and variables without values.

## 🔄 Main Flow Diagram

```
                            📋 INPUT: export command
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          🎯 my_export(char *arg, char ***env)               │
│                                                                             │
│  Input: arg = "VARIABLE" or "VARIABLE=value"                               │
│  Output: Success (1) or Failure (0)                                        │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔍 equal_sign = ft_strchr(arg, '=')                      │
│                                                                             │
│  Check if argument contains '=' character                                   │
│  This determines the main execution path                                    │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         ▼
        ┌───────────┐ ┌───────────┐ 
        │equal_sign │ │equal_sign │ 
        │== NULL    │ │!= NULL    │ 
        └───────────┘ └───────────┘ 
                │         │         
                ▼         ▼         
┌─────────────────┐ ┌─────────────────┐
│🚫 NO VALUE CASE│ │✅ WITH VALUE    │
│export VAR       │ │export VAR=val   │
└─────────────────┘ └─────────────────┘
```

## 🚫 Case 1: Export Without Value (`export VARIABLE`)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        🚫 NO VALUE CASE FLOW                               │
└─────────────────────────────────────────────────────────────────────────────┘

Input Examples: export PATH, export USER, export INVALID-VAR

                          equal_sign == NULL
                                 │
                                 ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔍 is_valid_identifier(arg)                             │
│                                                                             │
│  Validation Rules:                                                          │
│  • First char: letter (a-z, A-Z) or underscore (_)                        │
│  • Remaining chars: letters, digits (0-9), or underscore (_)               │
│  • Cannot be empty                                                         │
│  • Cannot start with digit                                                 │
│                                                                             │
│  Valid Examples: VAR, _VAR, VAR123, MY_VAR                                │
│  Invalid Examples: 123VAR, VAR-NAME, VAR@HOME, ""                         │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         
        ┌───────────┐ ┌───────────┐ 
        │  INVALID  │ │   VALID   │ 
        │IDENTIFIER │ │IDENTIFIER │ 
        └───────────┘ └───────────┘ 
                │         │         
                ▼         ▼         
┌─────────────────────────────────────────────────────────────────────────────┐
│                    ❌ handle_invalid_identifier_error()                     │
│                                                                             │
│  Actions:                                                                  │
│  • write(2, "minishell: export: `", 20)                                   │
│  • write(2, arg, ft_strlen(arg))                                           │
│  • write(2, "': not a valid identifier\n", 26)                            │
│  • set_status(1)                                                           │
│  • return (0)                                                              │
│                                                                             │
│  Output: "minishell: export: `INVALID-VAR': not a valid identifier"       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
                                                  │
                                                  ▼
                                            🔚 RETURN 0
                │
                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      ✅ export_without_value(arg, env)                      │
│                                                                             │
│  Purpose: Add variable name to exported variables list                     │
│  Note: Does NOT add to environment if not already present                  │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔍 var_already_exists(name, *env, len)                   │
│                                                                             │
│  Check if variable already exists in environment:                          │
│  • Search through env array                                                │
│  • Compare first 'len' characters                                          │
│  • Check if followed by '=' or end of string                              │
│                                                                             │
│  Examples:                                                                 │
│  • If env contains "PATH=/bin:/usr/bin"                                    │
│  • export PATH → var_already_exists returns 1                             │
│  • export NEWVAR → var_already_exists returns 0                           │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         
        ┌───────────┐ ┌───────────┐ 
        │ EXISTS IN │ │NOT IN ENV │ 
        │    ENV    │ │           │ 
        └───────────┘ └───────────┘ 
                │         │         
                ▼         ▼         
┌─────────────────┐ ┌─────────────────┐
│✅ return (1)    │ │📝 add_exported  │
│Do nothing more  │ │   _var(name)    │
│Variable already │ │Mark for export  │
│exported         │ │✅ return (1)    │
└─────────────────┘ └─────────────────┘
```

## ✅ Case 2: Export With Value (`export VARIABLE=value`)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        ✅ WITH VALUE CASE FLOW                             │
└─────────────────────────────────────────────────────────────────────────────┘

Input Examples: export PATH=/bin, export USER=john, export VAR=""

                        equal_sign != NULL
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      🔄 export_with_value(arg, equal_sign, env)             │
│                                                                             │
│  Input Processing:                                                          │
│  • arg = "VARIABLE=value"                                                   │
│  • equal_sign = pointer to '=' character                                   │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        🔪 String Parsing                                   │
│                                                                             │
│  • name = ft_substr(arg, 0, equal_sign - arg)                             │
│    Extract variable name before '='                                        │
│  • value = ft_strdup(equal_sign + 1)                                       │
│    Extract value after '='                                                 │
│                                                                             │
│  Examples:                                                                 │
│  • "PATH=/bin:/usr/bin" → name="PATH", value="/bin:/usr/bin"              │
│  • "USER=" → name="USER", value=""                                        │
│  • "MY_VAR=hello world" → name="MY_VAR", value="hello world"              │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔍 is_valid_identifier(name)                            │
│                                                                             │
│  Same validation as Case 1, but only for the name part                    │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         
        ┌───────────┐ ┌───────────┐ 
        │  INVALID  │ │   VALID   │ 
        │   NAME    │ │   NAME    │ 
        └───────────┘ └───────────┘ 
                │         │         
                ▼         ▼         
┌─────────────────────────────────────────────────────────────────────────────┐
│                    ❌ handle_export_error()                                 │
│                                                                             │
│  Actions:                                                                  │
│  • write(2, "minishell: export: not a valid identifier\n", 43)            │
│  • free(name)                                                              │
│  • set_status(1)                                                           │
│  • return (0)                                                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
                                                  │
                                                  ▼
                                            🔚 RETURN 0
                │
                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   ✅ process_export_value(name, value, env)                 │
│                                                                             │
│  Main processing for valid variable with value                             │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      🗑️ remove_exported_var(name)                           │
│                                                                             │
│  Remove from exported variables list (will be re-added to env)             │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   🔄 update_existing_var(name, value, *env)                 │
│                                                                             │
│  Try to update existing environment variable                               │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         
        ┌───────────┐ ┌───────────┐ 
        │UPDATE     │ │UPDATE     │ 
        │SUCCESS    │ │FAILED     │ 
        │(found var)│ │(not found)│ 
        └───────────┘ └───────────┘ 
                │         │         
                ▼         ▼         
┌─────────────────┐ ┌─────────────────┐
│✅ Variable      │ │➕ add_new_      │
│updated in place │ │  variable()     │
│Clean up & exit  │ │Add to env array │
└─────────────────┘ └─────────────────┘
                │         │
                ▼         ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        🧹 Final Cleanup                                    │
│                                                                             │
│  • free(name)                                                              │
│  • free(value)                                                             │
│  • set_status(0)                                                           │
│  • return (1)                                                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 🔄 Update vs Add New Variable Process

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     UPDATE EXISTING VARIABLE PROCESS                       │
└─────────────────────────────────────────────────────────────────────────────┘

update_existing_var(name, value, env):

Step 1: Create new entry string
┌─────────────────────────────────────────────────────────────────────────────┐
│  • tmp = ft_strjoin(name, "=")           // "PATH="                        │
│  • new_entry = ft_strjoin(tmp, value)    // "PATH=/new/path"               │
│  • free(tmp)                                                               │
└─────────────────────────────────────────────────────────────────────────────┘

Step 2: Search and replace in environment array
┌─────────────────────────────────────────────────────────────────────────────┐
│  for each env[i]:                                                          │
│    if (strncmp(env[i], name, len) == 0 && env[i][len] == '='):            │
│      free(env[i])                     // Free old entry                    │
│      env[i] = new_entry              // Replace with new                   │
│      return 1                        // Success                            │
│                                                                             │
│  If not found:                                                             │
│    free(new_entry)                                                         │
│    return 0                          // Variable doesn't exist             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                       ADD NEW VARIABLE PROCESS                             │
└─────────────────────────────────────────────────────────────────────────────┘

add_new_variable(name, value, env):

Step 1: Create variable string
┌─────────────────────────────────────────────────────────────────────────────┐
│  • tmp = ft_strjoin(name, "=")           // "NEWVAR="                      │
│  • new_var = ft_strjoin(tmp, value)      // "NEWVAR=newvalue"              │
│  • free(tmp)                                                               │
└─────────────────────────────────────────────────────────────────────────────┘

Step 2: Add to environment array
┌─────────────────────────────────────────────────────────────────────────────┐
│  add_env_variable(new_var, env):                                           │
│                                                                             │
│  • Count existing environment variables                                    │
│  • Allocate new array with size (count + 2)                               │
│  • Copy all existing pointers to new array                                 │
│  • Add new_var at position [count]                                         │
│  • Set new array[count + 1] = NULL                                         │
│  • free(old env array)                                                     │
│  • Update env pointer to new array                                         │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 📊 Case Examples and Results

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                            EXAMPLE SCENARIOS                               │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 1: export PATH
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "PATH"                                                              │
│ Flow: No '=' → export_without_value                                        │
│ Result: PATH marked as exported variable (if not already in env)           │
│ Status: 0 (success)                                                        │
│ Environment: Unchanged                                                     │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 2: export PATH=/new/path
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "PATH=/new/path"                                                    │
│ Flow: Has '=' → export_with_value → update_existing_var                   │
│ Result: PATH updated to "/new/path" in environment                         │
│ Status: 0 (success)                                                        │
│ Environment: PATH variable updated                                          │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 3: export NEWVAR=hello
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "NEWVAR=hello"                                                      │
│ Flow: Has '=' → export_with_value → add_new_variable                      │
│ Result: NEWVAR=hello added to environment                                  │
│ Status: 0 (success)                                                        │
│ Environment: New variable added                                            │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 4: export 123INVALID
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "123INVALID"                                                        │
│ Flow: No '=' → export_without_value → invalid identifier                  │
│ Result: Error message printed                                              │
│ Status: 1 (error)                                                          │
│ Environment: Unchanged                                                     │
│ Output: "minishell: export: `123INVALID': not a valid identifier"         │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 5: export VAR-NAME=value
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "VAR-NAME=value"                                                    │
│ Flow: Has '=' → export_with_value → invalid identifier                    │
│ Result: Error message printed                                              │
│ Status: 1 (error)                                                          │
│ Environment: Unchanged                                                     │
│ Output: "minishell: export: not a valid identifier"                       │
└─────────────────────────────────────────────────────────────────────────────┘

Scenario 6: export VAR=""
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: "VAR="                                                              │
│ Flow: Has '=' → export_with_value → empty value                           │
│ Result: VAR= added/updated in environment                                  │
│ Status: 0 (success)                                                        │
│ Environment: VAR with empty value                                          │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 🎯 Key Points Summary

### ✅ Success Cases (return 1, status 0):
- Valid identifier without value (added to export list)
- Valid identifier with any value (added/updated in environment)
- Empty values are allowed (`VAR=""`)
- Variables already existing are properly updated

### ❌ Error Cases (return 0, status 1):
- Invalid identifiers (starting with digit, containing special chars)
- Memory allocation failures
- Empty variable names

### 🔄 Environment Management:
- **Without value**: Only marks variable as exported, doesn't modify environment
- **With value**: Always modifies environment (update existing or add new)
- **Memory safety**: Proper cleanup of allocated strings and arrays
- **Export tracking**: Separate list maintains which variables should be exported

This comprehensive flow ensures proper bash-compatible `export` behavior with robust error handling and memory management.
