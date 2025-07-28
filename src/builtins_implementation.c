/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_implementation.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 15:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

/* ========================================================================== */
/*                                 CD COMMAND                                */
/* ========================================================================== */

/**
 * Updates the PWD and OLDPWD environment variables
 */
static void update_pwd_vars(char *old_pwd, char *new_pwd, char ***env)
{
    int i;
    char *pwd_entry;
    char *oldpwd_entry;
    bool pwd_found = false;
    bool oldpwd_found = false;

    // Create new entries
    pwd_entry = ft_strjoin("PWD=", new_pwd);
    oldpwd_entry = ft_strjoin("OLDPWD=", old_pwd);
    
    if (!pwd_entry || !oldpwd_entry)
        return;

    // Update existing entries or add new ones
    i = 0;
    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], "PWD=", 4) == 0)
        {
            free((*env)[i]);
            (*env)[i] = pwd_entry;
            pwd_found = true;
        }
        else if (ft_strncmp((*env)[i], "OLDPWD=", 7) == 0)
        {
            free((*env)[i]);
            (*env)[i] = oldpwd_entry;
            oldpwd_found = true;
        }
        i++;
    }

    // If PWD or OLDPWD weren't found, we'd need to expand the env array
    // For simplicity, we'll just update existing ones
    if (!pwd_found)
        free(pwd_entry);
    if (!oldpwd_found)
        free(oldpwd_entry);
}

/**
 * Changes directory to HOME if no argument provided
 */
static int cd_home(char ***env)
{
    char *home;
    char *current_pwd;

    home = getenv("HOME");
    if (!home)
    {
        printf("minishell: cd: HOME not set\n");
        return (1);
    }

    current_pwd = getcwd(NULL, 0);
    if (chdir(home) != 0)
    {
        perror("minishell: cd");
        if (current_pwd)
            free(current_pwd);
        return (1);
    }

    if (current_pwd)
    {
        update_pwd_vars(current_pwd, home, env);
        free(current_pwd);
    }
    return (0);
}

/**
 * Implementation of the cd built-in command
 * Supports: cd, cd ~, cd -, cd <path>
 */
int builtin_cd(t_command *cmd, char ***env)
{
    char *path;
    char *current_pwd;
    char *new_pwd;
    struct stat st;

    if (!cmd->args[1])
        return (cd_home(env));

    path = cmd->args[1];

    // Handle cd -
    if (ft_strcmp(path, "-") == 0)
    {
        path = getenv("OLDPWD");
        if (!path)
        {
            printf("minishell: cd: OLDPWD not set\n");
            return (1);
        }
        printf("%s\n", path);
    }
    // Handle cd ~ or cd ~/path
    else if (path[0] == '~')
    {
        char *home = getenv("HOME");
        if (!home)
        {
            printf("minishell: cd: HOME not set\n");
            return (1);
        }
        if (path[1] == '\0')
            path = home;
        else if (path[1] == '/')
        {
            char *temp = ft_strjoin(home, path + 1);
            path = temp;
        }
    }

    // Check if path exists and is a directory
    if (stat(path, &st) != 0)
    {
        printf("minishell: cd: %s: No such file or directory\n", path);
        return (1);
    }

    if (!S_ISDIR(st.st_mode))
    {
        printf("minishell: cd: %s: Not a directory\n", path);
        return (1);
    }

    // Get current directory before changing
    current_pwd = getcwd(NULL, 0);

    // Change directory
    if (chdir(path) != 0)
    {
        perror("minishell: cd");
        if (current_pwd)
            free(current_pwd);
        return (1);
    }

    // Get new directory and update environment
    new_pwd = getcwd(NULL, 0);
    if (current_pwd && new_pwd)
        update_pwd_vars(current_pwd, new_pwd, env);

    if (current_pwd)
        free(current_pwd);
    if (new_pwd)
        free(new_pwd);

    return (0);
}

/* ========================================================================== */
/*                                PWD COMMAND                                */
/* ========================================================================== */

/**
 * Implementation of the pwd built-in command
 * Prints the current working directory
 */
int builtin_pwd(t_command *cmd)
{
    char *pwd;

    (void)cmd; // pwd takes no arguments

    pwd = getcwd(NULL, 0);
    if (!pwd)
    {
        perror("minishell: pwd");
        return (1);
    }

    printf("%s\n", pwd);
    free(pwd);
    return (0);
}

/* ========================================================================== */
/*                                ENV COMMAND                                */
/* ========================================================================== */

/**
 * Implementation of the env built-in command
 * Prints all environment variables
 */
int builtin_env(t_command *cmd, char **env)
{
    int i;

    (void)cmd; // env takes no arguments in this implementation

    if (!env)
        return (1);

    i = 0;
    while (env[i])
    {
        printf("%s\n", env[i]);
        i++;
    }

    return (0);
}

/* ========================================================================== */
/*                              EXPORT COMMAND                               */
/* ========================================================================== */

/**
 * Counts the number of environment variables
 */
static int count_env_vars(char **env)
{
    int count = 0;

    while (env[count])
        count++;
    return (count);
}

/**
 * Creates a new environment array with an additional variable
 */
static char **add_env_var(char **env, char *new_var)
{
    int count;
    char **new_env;
    int i;

    count = count_env_vars(env);
    new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
        return (NULL);

    i = 0;
    while (env[i])
    {
        new_env[i] = ft_strdup(env[i]);
        if (!new_env[i])
        {
            // Free previously allocated strings on error
            while (--i >= 0)
                free(new_env[i]);
            free(new_env);
            return (NULL);
        }
        i++;
    }

    new_env[i] = ft_strdup(new_var);
    if (!new_env[i])
    {
        while (--i >= 0)
            free(new_env[i]);
        free(new_env);
        return (NULL);
    }
    new_env[i + 1] = NULL;

    return (new_env);
}

/**
 * Updates an existing environment variable
 */
static int update_env_var(char **env, char *var_name, char *new_var)
{
    int i;
    size_t name_len;

    name_len = ft_strlen(var_name);
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], var_name, name_len) == 0 && env[i][name_len] == '=')
        {
            free(env[i]);
            env[i] = ft_strdup(new_var);
            return (env[i] ? 0 : 1);
        }
        i++;
    }
    return (-1); // Not found
}

/**
 * Validates if a variable name is valid (alphanumeric + underscore, starts with letter/underscore)
 */
static int is_valid_identifier(char *name)
{
    int i;

    if (!name || !*name)
        return (0);

    if (!ft_isalpha(name[0]) && name[0] != '_')
        return (0);

    i = 1;
    while (name[i])
    {
        if (!ft_isalnum(name[i]) && name[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

/**
 * Prints environment variables in export format
 */
static void print_export_vars(char **env)
{
    int i;
    char *equals_pos;

    i = 0;
    while (env[i])
    {
        printf("declare -x ");
        equals_pos = ft_strchr(env[i], '=');
        if (equals_pos)
        {
            *equals_pos = '\0';
            printf("%s=\"%s\"\n", env[i], equals_pos + 1);
            *equals_pos = '=';
        }
        else
        {
            printf("%s\n", env[i]);
        }
        i++;
    }
}

/**
 * Implementation of the export built-in command
 * Supports: export (list vars), export VAR=value
 */
int builtin_export(t_command *cmd, char ***env)
{
    int i;
    char *equals_pos;
    char *var_name;
    char **new_env;

    // If no arguments, print all exported variables
    if (!cmd->args[1])
    {
        print_export_vars(*env);
        return (0);
    }

    // Process each argument
    i = 1;
    while (cmd->args[i])
    {
        equals_pos = ft_strchr(cmd->args[i], '=');
        
        if (equals_pos)
        {
            // Extract variable name
            var_name = ft_substr(cmd->args[i], 0, equals_pos - cmd->args[i]);
            if (!var_name)
                return (1);

            if (!is_valid_identifier(var_name))
            {
                printf("minishell: export: `%s': not a valid identifier\n", cmd->args[i]);
                free(var_name);
                return (1);
            }

            // Try to update existing variable
            if (update_env_var(*env, var_name, cmd->args[i]) == -1)
            {
                // Variable doesn't exist, add it
                new_env = add_env_var(*env, cmd->args[i]);
                if (!new_env)
                {
                    free(var_name);
                    return (1);
                }
                
                // Free old environment (if it was dynamically allocated)
                // Note: In a real implementation, you'd need to track whether
                // the environment was dynamically allocated
                *env = new_env;
            }
            free(var_name);
        }
        else
        {
            // Just the variable name without value
            if (!is_valid_identifier(cmd->args[i]))
            {
                printf("minishell: export: `%s': not a valid identifier\n", cmd->args[i]);
                return (1);
            }
            // In bash, this would mark the variable for export even without a value
            // For simplicity, we'll just ignore it here
        }
        i++;
    }

    return (0);
}

/* ========================================================================== */
/*                               UNSET COMMAND                               */
/* ========================================================================== */

/**
 * Removes a variable from the environment
 */
static char **remove_env_var(char **env, char *var_name)
{
    int count;
    int i, j;
    char **new_env;
    size_t name_len;

    count = count_env_vars(env);
    name_len = ft_strlen(var_name);

    // Count how many variables will remain
    i = 0;
    j = 0;
    while (env[i])
    {
        if (!(ft_strncmp(env[i], var_name, name_len) == 0 && env[i][name_len] == '='))
            j++;
        i++;
    }

    new_env = malloc(sizeof(char *) * (j + 1));
    if (!new_env)
        return (NULL);

    // Copy variables except the one being removed
    i = 0;
    j = 0;
    while (env[i])
    {
        if (!(ft_strncmp(env[i], var_name, name_len) == 0 && env[i][name_len] == '='))
        {
            new_env[j] = ft_strdup(env[i]);
            if (!new_env[j])
            {
                // Free on error
                while (--j >= 0)
                    free(new_env[j]);
                free(new_env);
                return (NULL);
            }
            j++;
        }
        i++;
    }
    new_env[j] = NULL;

    return (new_env);
}

/**
 * Implementation of the unset built-in command
 * Removes environment variables
 */
int builtin_unset(t_command *cmd, char ***env)
{
    int i;
    char **new_env;

    if (!cmd->args[1])
        return (0); // No variables to unset

    i = 1;
    while (cmd->args[i])
    {
        if (!is_valid_identifier(cmd->args[i]))
        {
            printf("minishell: unset: `%s': not a valid identifier\n", cmd->args[i]);
            return (1);
        }

        new_env = remove_env_var(*env, cmd->args[i]);
        if (new_env)
        {
            // Free old environment (if it was dynamically allocated)
            // Note: In a real implementation, you'd need to track this
            *env = new_env;
        }
        i++;
    }

    return (0);
}

/* ========================================================================== */
/*                            UTILITY FUNCTIONS                              */
/* ========================================================================== */

/**
 * Enhanced built_in function that recognizes all built-in commands
 * This should replace the existing built_in function
 */
bool is_builtin_command(char *cmd)
{
    if (!cmd)
        return (false);

    if (ft_strcmp(cmd, "echo") == 0)
        return (true);
    if (ft_strcmp(cmd, "cd") == 0)
        return (true);
    if (ft_strcmp(cmd, "pwd") == 0)
        return (true);
    if (ft_strcmp(cmd, "export") == 0)
        return (true);
    if (ft_strcmp(cmd, "unset") == 0)
        return (true);
    if (ft_strcmp(cmd, "env") == 0)
        return (true);
    if (ft_strcmp(cmd, "exit") == 0)
        return (true);

    return (false);
}

/**
 * Executes the appropriate built-in command
 * Returns the exit status of the command
 */
int execute_builtin(t_command *cmd, char ***env, t_data **data)
{
    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);

    if (ft_strcmp(cmd->args[0], "echo") == 0)
    {
        my_echo(cmd);
        return (0);
    }
    else if (ft_strcmp(cmd->args[0], "cd") == 0)
    {
        return (builtin_cd(cmd, env));
    }
    else if (ft_strcmp(cmd->args[0], "pwd") == 0)
    {
        return (builtin_pwd(cmd));
    }
    else if (ft_strcmp(cmd->args[0], "export") == 0)
    {
        return (builtin_export(cmd, env));
    }
    else if (ft_strcmp(cmd->args[0], "unset") == 0)
    {
        return (builtin_unset(cmd, env));
    }
    else if (ft_strcmp(cmd->args[0], "env") == 0)
    {
        return (builtin_env(cmd, *env));
    }
    else if (ft_strcmp(cmd->args[0], "exit") == 0)
    {
        my_exit(&cmd);
        return (0);
    }

    return (1); // Unknown built-in
}

/* ========================================================================== */
/*                        INTEGRATION INSTRUCTIONS                           */
/* ========================================================================== */

/*
 * TO INTEGRATE THESE BUILT-INS INTO YOUR MAIN CODE:
 * 
 * 1. Add the function declarations to minishell.h:
 *    - int builtin_cd(t_command *cmd, char ***env);
 *    - int builtin_pwd(t_command *cmd);
 *    - int builtin_env(t_command *cmd, char **env);
 *    - int builtin_export(t_command *cmd, char ***env);
 *    - int builtin_unset(t_command *cmd, char ***env);
 *    - bool is_builtin_command(char *cmd);
 *    - int execute_builtin(t_command *cmd, char ***env, t_data **data);
 * 
 * 2. Update your Makefile to include this file:
 *    Add "src/builtins_implementation.c" to your SRC variable
 * 
 * 3. Replace the existing built_in() function with is_builtin_command()
 * 
 * 4. In your command execution logic, check for built-ins first:
 *    if (is_builtin_command(cmd->args[0]))
 *        exit_status = execute_builtin(cmd, &env, &data);
 *    else
 *        // execute external command
 * 
 * 5. Note: These functions assume you have ft_strcmp available.
 *    If you don't, add it to your libft or use ft_strncmp with length.
 * 
 * 6. The export and unset functions modify the environment array.
 *    Make sure your main function passes the environment as char ***
 *    so it can be modified.
 */
