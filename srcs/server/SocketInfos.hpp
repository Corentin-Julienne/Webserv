/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketInfos.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 10:20:55 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/06 12:31:16 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETINFOS_HPP
# define SOCKETINFOS_HPP

struct SocketInfos
{   
    std::string                         reqType;
    std::string                         uri;
    std::string                         body;
    std::map<std::string, std::string>	headers;
    std::string                         absoluteURIPath;
    std::string                         queryString;
    std::string                         path;
    std::string                         locPath;
};

#endif
