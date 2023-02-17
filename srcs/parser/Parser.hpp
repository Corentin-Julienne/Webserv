/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:40 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/17 21:46:48 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>

class Parser
{
	
	public:
	
		Parser(char *argv);
		~Parser();

	private:
	
		void	_openFile(char *argv);
		bool	_isFileValid(void);
		void	_ifstreamToStr(void);
		void	_iterateThroughStr(void);
		int		_splitServerBlock(int i);
		bool	_isServerBlockValid(std::string substr);

		std::ifstream				_conf_file;
		std::string					_conf_str;
		std::vector<std::string>	_server_blocks;
};
