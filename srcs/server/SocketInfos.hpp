/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketInfos.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 10:20:55 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/07 17:23:25 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETINFOS_HPP
# define SOCKETINFOS_HPP

# include <vector>

struct SocketInfos
{   
    std::string                         reqType;
    std::string                         uri;
    std::vector<char>                   body;
    std::map<std::string, std::string>	headers;
    std::string                         absoluteURIPath;
    std::string                         queryString;
    std::string                         path;
    std::string                         locPath;
};

#endif
