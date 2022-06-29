/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Broadcom autocomplete
 */


#ifdef INCLUDE_AUTOCOMPLETE

#include <sal/appl/editline/autocomplete.h>
#include <sal/appl/sal.h>
#include <appl/diag/system.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <sal/core/alloc.h>
#include <soc/drv.h>
#include <sys/ioctl.h>

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

autocomplete_init_t autocomplete_init[SOC_MAX_NUM_DEVICES];

char autocomplete_string[SOC_MAX_NUM_DEVICES][AUTOCOMPLETE_MAX_STRING_LEN];

static autocomplete_node_t autocomplete_root[SOC_MAX_NUM_DEVICES];

autocomplete_node_t *autocomplete_find_root(int unit, char *keyword) {

    autocomplete_node_t *node = autocomplete_root[unit].first_child;

    while (node != NULL) {
        if (sal_strcasecmp(node->keyword, keyword) == 0) {
            return node;
        }
        node = node->next_sibling;
    }

    return NULL;
}

void autocomplete_init_cb_set(int unit, sal_autocomplete_init_invoke_cb_t autocomplete_init_invoke_cb, void* autocomplete_init_cmds)
{
    autocomplete_init[unit].autocomplete_init_cb = autocomplete_init_invoke_cb;
    autocomplete_init[unit].autocomplete_init_cmds = autocomplete_init_cmds;
}


autocomplete_node_t *autocomplete_find_node(int unit, char **keywords) {
    autocomplete_node_t *node = autocomplete_root[unit].first_child;
    int i = 0;

    while (keywords[i] != NULL) {
        while (node != NULL) {
            if (sal_strcasecmp(node->keyword, keywords[i]) == 0) {
                if(keywords[i+1] == NULL) {
                    return node;
                }
                node = node->first_child;
                break;
            }
            node = node->next_sibling;
        }
        if (node == NULL) return NULL;
        i++;
    }

    return NULL;
}

/* for description see the header include/sal/appl/editline/autocomplete.h */

autocomplete_node_t *autocomplete_node_add(int unit, autocomplete_node_t *parent, char *keyword, char is_option, sh_sand_invoke_cb_t invoke_cb, sh_sand_ac_option_cb_t ac_option_cb) {

    autocomplete_node_t *node = (autocomplete_node_t*) sal_alloc(sizeof(*node), "autocomplete node");

    sal_memset(node,0,sizeof(*node));
    node->keyword = sal_strdup(keyword);
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->parent = parent;
    node->is_option = is_option;
    node->invoke_cb = invoke_cb;
    node->ac_option_cb = ac_option_cb;

    /* check if parent is root */
    if (parent == NULL) {
        parent = &autocomplete_root[unit];
        node->parent = parent;
    }

    /* check if it should be the first child of parent */
    if (parent->first_child == NULL) {
        parent->first_child = node;
    } else {
        autocomplete_node_t* last_child = parent->first_child;
        while(last_child->next_sibling != NULL) {
            last_child = last_child->next_sibling;
        }
        last_child->next_sibling = node;
    }

    return node;
}

/* for description see the header include/sal/appl/editline/autocomplete.h */

void autocomplete_delete_all(int unit) {

    autocomplete_node_t *node = autocomplete_root[unit].first_child;
    autocomplete_node_t *next_node;
    while (node != NULL) {
        next_node = node->next_sibling;
        autocomplete_node_delete(unit, node);
        node = next_node;
    }
    autocomplete_root[unit].first_child = NULL;

}

/* for description see the header include/sal/appl/editline/autocomplete.h */

void autocomplete_node_delete(int unit, autocomplete_node_t* node) {

    autocomplete_node_t *parent;
    autocomplete_node_t *child;

    if (node == NULL)
        return;

    parent = node->parent;
    child = node->first_child;

    /* check if the node is the first child of it's parent */
    if (parent->first_child == node) {
        parent->first_child = node->next_sibling;
    } else {
        autocomplete_node_t* older_sibling = parent->first_child;
        while(older_sibling->next_sibling != node) {
            older_sibling = older_sibling->next_sibling;
        }
        older_sibling->next_sibling = node->next_sibling;
    }

    /* delete all of the node's children */
    while (child != NULL) {
        autocomplete_node_t* next_child = child->next_sibling;
        autocomplete_node_delete(unit, child);
        child = next_child;
    }

    /* delete the node */
    sal_free(node->keyword);
    sal_free(node);

    return;
}

/* get a prefix as a parameter and a new word, calcuate the largest common prefix and save it in prefix */

static void update_common_prefix(int index, char *prefix, char *new_word) {
    /* if the new_word is the first word */
    if (index == 0) {
        sal_strncpy(prefix, new_word, AUTOCOMPLETE_MAX_STRING_LEN-1);
    } else {
        int i=0;
        while (prefix[i] == new_word[i]) {
            i++;
        }
        /* truncate the prefix */
        sal_memset(prefix+i, 0, sal_strlen(prefix+i));
    }
}


static void uncheck_options (autocomplete_node_t *options_node) {
    while(options_node != NULL) {
        options_node->is_checked = 0;
        options_node = options_node->next_sibling;
    }
    return;
}

/* for description see the header include/sal/appl/editline/autocomplete.h */

char* autocomplete_print(int unit, char* input, char* prompt) {
   autocomplete_node_t *prev_node = NULL;
   autocomplete_node_t *options_node = NULL;
   autocomplete_node_t *node = NULL;
   char new_line[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char str_to_print[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char str_tmp[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char common_prefix[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char *prefix = "";
   const char separator[2] = " ";
   char *token;
   int token_idx=0;
   int node_depth=0;
   int num_of_suggestions = 0;
   char str_cpy[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   int max_autocomplete_str_len = 0;
   int accumulative_line_len = 0;
   struct winsize window_size;
   int window_len = 0;
   char is_last_token = 0;
   char *last_token;
   char is_option = 0;
   char print_options = 0;
   int is_ctest = FALSE;
   char *tokstr;

   if (unit == -1) {
       sal_printf("\nError: autocomplete failed. No attached unit found.\n");
       sal_printf("%s", prompt);
       return "";
   }

   /*
    * If the autocomplete tree is not yet initialized, attempt to initialize it.
    */
   if (autocomplete_root[unit].first_child == NULL)
   {
       /*
        * Attempt to initialize autocomplete tree - can only be accomplished if callback and initial collection of commands exist
        */
       if (autocomplete_init[unit].autocomplete_init_cb != NULL && autocomplete_init[unit].autocomplete_init_cmds != NULL)
       {
           autocomplete_init[unit].autocomplete_init_cb(unit, autocomplete_init[unit].autocomplete_init_cmds);
       }
       else
       {
           return "";
       }
   }
   node = prev_node = autocomplete_root[unit].first_child;

   if (node == NULL) {
       return "";
   }

   /* autocomplete only in BCM shell */
   if (sal_strstr(prompt, "BCM") == NULL) {
       return "";
   }

   sal_strncpy(str_cpy, input, AUTOCOMPLETE_MAX_STRING_LEN-1);

   /* get the first token */
   token = sal_strtok_r(str_cpy, separator, &tokstr);

   /* checks to see if the first token is ctest, is_ctest used to prevent invoking create test lists callback */
   if (token && sal_strncasecmp(token, "ctest", 5) == 0)
   {
       is_ctest = TRUE;
   }

   /* walk through other tokens */
   while( token != NULL )
   {
      char* assignment = NULL;
      token_idx++;
      prev_node = node;
      is_option = 0;
      if (node != NULL && node->is_option) {
          print_options = 1;
          options_node = node;
      }
      while(node != NULL && node->keyword != NULL) {
          /* if the command is not ctest, need to check invoke_cb to determine if node command/option is active */
          if (sal_strcasecmp(node->keyword, token) == 0 && (is_ctest == TRUE || node->invoke_cb == NULL || node->invoke_cb(unit, NULL) == FALSE)) {

              if(node->is_option) {
                  node->is_checked = 1;
              }

              if (print_options && node->first_child == NULL) {
                  node = options_node;
              } else {
                  node = node->first_child;
              }

              node_depth++;
              break;
          }
          assignment = strchr(token, '=');
          if (assignment != NULL) {
              if (sal_strncasecmp(node->keyword, token, MIN(assignment - token + 1, sal_strlen(node->keyword))) == 0) {
                  is_option = 1;

                  node->is_checked = 1;

                  if (node->first_child != NULL) {
                      node = node->first_child;
                  }
                  else if (node->ac_option_cb != 0)
                  {
                      char *ac_substring;
                      char ac_full_string[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
                      int ii;
                      ac_substring = strchr(assignment, assignment[1]);
                      node->ac_option_cb(unit, ac_substring, ac_full_string, &num_of_suggestions);
                      if (num_of_suggestions == 1)
                      {
                          /*
                           * if the number of suggestions is exactly 1, and ac_full_string is empty
                           * or it is the same length as the user provided string (possibly not including an additional space character at the end),
                           * then there is nothing to auto-complete
                           * instead, print remaining sibling options to console
                           */
                          if ((ac_full_string[0] == 0) || (sal_strlen(ac_full_string) - sal_strlen(ac_substring) <= 1))
                          {
                              /* if either of above conditions are true, a full match already exists, the remaining options (siblings) should be shown */
                              num_of_suggestions = 0;
                              node_depth++;
                              is_option = 0;
                              break;
                          }
                          /* find index of first difference between desired full string and substring provided by user */
                          for (ii = 0; tolower(ac_substring[ii]) == tolower(ac_full_string[ii]); ii++);
                          sal_strcpy(autocomplete_string[unit], "");
                          /*
                           * if substring provided by user is beginning of full string, auto-complete inline
                           * else edit input to enable auto-complete in the line below
                           */
                          if (ii == sal_strlen(ac_substring))
                          {
                              sal_strncat_s(autocomplete_string[unit], ac_full_string + sal_strlen(ac_substring), sizeof(autocomplete_string[unit]));
                          }
                          else
                          {
                              /* replace the user-submitted substring with the beginning characters of the full string */
                              for (ii = 0; ii < sal_strlen(assignment) - 1; ii++)
                              {
                                  input[sal_strlen(input) - sal_strlen(assignment) + ii + 1] = ac_full_string[ii];
                              }
                              sal_printf("\n%s%s", prompt, input);
                              /* set the returned auto-complete string to be the completion of the full string after editing the user-provided input */
                              sal_strncat_s(autocomplete_string[unit], ac_full_string + sal_strlen(assignment) - 1, sizeof(autocomplete_string[unit]));
                          }
                          return autocomplete_string[unit];
                      }
                      else if (num_of_suggestions > 1)
                      {
                          char *ac_option;
                          int common_prefix_length;
                          ac_option = sal_strtok_r(ac_full_string, separator, &tokstr);
                          /*
                           * find the minimal length of common_prefix for it to be relevant,
                           * defined to be the index where the user provided substring begins in the first option
                           */
                          common_prefix_length = sal_strncaseindex(ac_option, ac_substring, AUTOCOMPLETE_MAX_STRING_LEN, AUTOCOMPLETE_MAX_STRING_LEN) - ac_option;
                          prefix = ac_substring;
                          update_common_prefix(0, common_prefix, ac_option);
                          while (ac_option != NULL)
                          {
                              /* iterate through all partial matches, add them to str_to_print and find longest common prefix */
                              update_common_prefix(num_of_suggestions, common_prefix, ac_option);
                              if (max_autocomplete_str_len < sal_strlen(ac_option))
                              {
                                  max_autocomplete_str_len = sal_strlen(ac_option);
                              }
                              sal_strncat_s(str_to_print, ac_option, sizeof(str_to_print));
                              sal_strncat_s(str_to_print, separator, sizeof(str_to_print));
                              ac_option = sal_strtok_r(NULL, separator, &tokstr);
                          }
                          /*
                           * if the length of common_prefix is smaller than common_prefix_length, then we cannot complete all characters up to the provided substring
                           * thus there is no relevant common prefix to use
                           */
                          if (sal_strlen(common_prefix) < common_prefix_length)
                          {
                              common_prefix[0] = 0;
                          }
                          for (ii = 0; ii < sal_strlen(assignment) - 1; ii++)
                          {
                              /* if relevant, replace the user-submitted substring with the beginning characters of all matches */
                              if (common_prefix[ii] == 0) break;
                              input[sal_strlen(input) - sal_strlen(assignment) + ii + 1] = common_prefix[ii];
                          }
                          break;
                      }
                      /*
                       * if num_of_suggestions equals AUTOCOMPLETE_PRINT_OUT_OF_SCOPE,
                       * assume the relevant options have already been printed to the console
                       * reprint the input back to the console to enable user to continue writing commands/options
                       */
                      else if (num_of_suggestions == AUTOCOMPLETE_PRINT_OUT_OF_SCOPE)
                      {
                          sal_printf("%s%s", prompt, input);
                          break;
                      }
                  }
                  else {
                      node = options_node;
                  }

                  node_depth++;
                  break;
              }
          }
          node = node->next_sibling;
      }
      if (token_idx != node_depth) break;
      if(is_option) {
          last_token = token;
          token = sal_strtok_r(NULL, separator, &tokstr);
          /* if last token */
          if (token == NULL && node != options_node) {
              sal_strncat_s(new_line, last_token, sizeof(new_line));
              token = assignment + 1;
              is_last_token = 1;
          } else {
              sal_strncat_s(new_line, last_token, sizeof(new_line));
              node = options_node;
          }
          continue;
      } else {
          sal_strncat_s(new_line,token, sizeof(new_line));
      }

      if(token[strlen(token)-1] != '=') {
          sal_strncat_s(new_line," ", sizeof(new_line));
      }

      if (is_last_token) {
          token = NULL;
      } else {
          token = sal_strtok_r(NULL, separator, &tokstr);
      }
   }

   /* no suggestions */
   if (token_idx != node_depth && sal_strtok_r(NULL, separator, &tokstr)) {
       uncheck_options(options_node);
       return "";
   }

   /* if num_of_suggestions is strictly positive, prefix and suggestions have already been set */
   if (!(num_of_suggestions > 0))
   {
       /* show all options */
      if (token_idx == node_depth) {
          prefix = "";
      } else { /* complete by the prefix */
          node = prev_node;
          prefix = token;
      }

       /* walk through the suggestions */
       while(node != NULL && node->keyword != NULL) {
           if (sal_strncasecmp(node->keyword, prefix, sal_strlen(prefix)) == 0 && !node->is_checked) {
               /* if the command is not ctest, need to check invoke_cb to determine if node command/option is active */
               if (is_ctest == TRUE || node->invoke_cb == NULL || node->invoke_cb(unit, NULL) == FALSE)
               {
                   update_common_prefix(num_of_suggestions, common_prefix, node->keyword);
                   sal_sprintf(str_tmp, "%s ", node->keyword);
                   if (max_autocomplete_str_len < sal_strlen(str_tmp)) {
                       max_autocomplete_str_len = sal_strlen(str_tmp);
                   }
                   sal_strncat_s(str_to_print, str_tmp, sizeof(str_to_print));
                   sal_strncat_s(new_line,str_tmp, sizeof(str_to_print));
                   num_of_suggestions++;
               }
           }
           node = node->next_sibling;
       }
   }
   uncheck_options(options_node);

   sal_strcpy(autocomplete_string[unit],"");

   if (num_of_suggestions > 0 && token_idx == node_depth &&
       sal_strlen(input) > 0 && input[sal_strlen(input)-1] != ' ' &&
       input[sal_strlen(input)-1] != '=') {
           sal_strncat_s(autocomplete_string[unit]," ", sizeof(autocomplete_string[unit]));
   }

   if (num_of_suggestions > 1) {
       ioctl(0, TIOCGWINSZ, &window_size);
       window_len = window_size.ws_col;
       sal_printf("\n");
       token = sal_strtok_r(str_to_print, separator, &tokstr);
       while( token != NULL )
       {
           accumulative_line_len += max_autocomplete_str_len + 1;
           if (accumulative_line_len >= window_len) {
               sal_printf("\n");
               accumulative_line_len = max_autocomplete_str_len;
           }
           sal_printf("%*s ", -max_autocomplete_str_len, token);
           token = sal_strtok_r(NULL, separator, &tokstr);
       }
       sal_printf("\n");
       sal_printf("%s%s", prompt, input);
   }
   sal_strncat_s(autocomplete_string[unit], common_prefix + sal_strlen(prefix), sizeof(autocomplete_string[unit]));

   return autocomplete_string[unit];
}

#else /* INCLUDE_AUTOCOMPLETE */
typedef int _autocomplete_autocomplete_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_AUTOCOMPLETE */

