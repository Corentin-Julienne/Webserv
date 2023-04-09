#ifndef __INCLUDE_HPP__
# define __INCLUDE_HPP__

# include <cstdlib>
# include <iostream>
# include "../parser/Parser.hpp"

bool	isDirectory(const std::string &path);
void	call_error(std::string failing_call, bool exit_process);

#endif
