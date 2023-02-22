/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 16:20:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/22 12:44:53 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"
// check if everything is necessary
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>

struct Location
{
	public:

		Location(void);
		~Location();

		void	addBack(void);
		void	addSub(int nesting_lvl);
		void	enforceInheritance(void);

	public:

		struct Location			*prev;
		struct Location			*next;
		struct Location			*sub;
		struct Location			*sup;
		int						_nesting_lvl;
};
