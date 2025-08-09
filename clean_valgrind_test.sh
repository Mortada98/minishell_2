#!/bin/bash

# Clean Environment Valgrind Test
# Run this outside VS Code for cleaner results

export EDITOR=""
export VISUAL=""
unset VSCODE_PID
unset VSCODE_IPC_HOOK_CLI

echo "Running valgrind test in clean environment..."
echo "echo 'Hello World'; pwd; ls | head -3; exit" | valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.sup ./minishell

echo -e "\nFor comprehensive testing in clean environment:"
echo "./test_valgrind_leaks.sh"
