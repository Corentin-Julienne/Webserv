/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 16:20:39 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/20 15:26:22 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) : prev(nullptr), next(nullptr), sub(nullptr), sup(nullptr), _nesting_lvl(0)
{
	// add relevant info
	
	// increment nesting level if necessary
	Location	*iterator = this;
	
	while (this->sup != nullptr)
	{
		iterator = iterator->sup;
		this->_nesting_lvl++;
	}
	// enforce inheritance
	this->enforceInheritance();
}

Location::~Location()
{
	
}

void	Location::addBack(void) // to test
{
	Location	*iterator = this;
	Location	newLocation;

	while (iterator->next != nullptr)
		iterator = iterator->next;
	iterator->next = &newLocation;
	iterator->next->prev = iterator;
}

void	Location::addSub(int nesting_lvl) // to test
{
	Location	*iterator = this;
	Location	newLocation;

	while (iterator->next && iterator->_nesting_lvl != nesting_lvl)
		iterator = iterator->next;
}

void	Location::enforceInheritance(void)
{
	// TODO
}
