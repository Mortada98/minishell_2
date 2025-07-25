

// void	handle_word_token(t_token **token, int start, char *line, int *i, t_data **data)
// {
//   bool  should_join = false;
//   t_token *new;
//   int flag = 0;
//   char *str, *word;
//   t_token_type value = TOKEN_WORD;
//
//   check_the_last_element(token, data);
//   if (*i > start)
//   {
//     mix(&should_join, line, i);
//     word = make_content(line, i, start, data);
//     if (!word)
//       return;
//     convert_exit_status(word);
//     if (word && *word != '\0')
//     {
//       if (!(*data)->should_expand_outside)
//         str = expand_env(word);
//       else
//         str = word;
//       join_expansion(str, token);
//       if (str[0] == '"' || str[0] == '\'')
//       {
//         // value = get_token_type(str);
//         str = remove_quotes(str);
//         flag = 1;
//       }
//       if (str && str != word)
//       {
//         free(word);
//         word = str;
//       }
//       if (!flag)
//         value = get_token_type(word);
//       if (!flag && (ft_strchr(word, ' ') || ft_strchr(word, '\t')))
//       {
//         make_list(word, token, value);
//         free(word);
//         return;
//       }
//       new = creat_token(word, value, should_join);
//       if (new)
//         add_token(token, new);
//       free(word);
//     }
//   }
// }
