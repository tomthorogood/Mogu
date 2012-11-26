/*
 * GroupManager.h
 *
 *  Created on: Nov 20, 2012
 *      Author: tom
 */

#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_

#include <Static.h>
#include <Mogu.h>

/*!\brief The GroupManager class queries information about groups,
 * performing the calculations necessary to retrieve members of certain levels
 * and change memberships as needed.
 */

class GroupManager
{
	const std::string& __id;
	std::vector <std::string> membershipQuery;
	std::string GROUP_PREFIX;
	std::string MEMBERSHIP;
	Mogu* app;

	inline std::vector <std::string>& __getMembership__(int max, int min)
	{
		membershipQuery.clear();
		app->redisExec(Mogu::Keep, "zrevrangebyscore %s %d %d",
				MEMBERSHIP.c_str(), max, min);
		Redis::toVector(app->reply(), membershipQuery);
		return membershipQuery;
	}

	void __init__();

public:
	/*!\brief Standard constructor. Requires a groupid
	 * @param groupid The groupid of the group.
	 */
	GroupManager (const std::string& groupid);

	/*!\brief Populates the membership query with a list of all members. */
	std::vector <std::string>& getMembership();

	/*!\brief Populates the membership query with a list of all members between
	 * ranks [low] and ranks [high].
	 * @param max The highest priveleged member to retrieve.
	 * @param min The lowest priveleged member to retrieve.
	 * @return A reference to the membership query vector.
	 */
	std::vector <std::string>& getMembership(
			const int& max, const int& min);

	/*!\brief Populates the membership query with a list of all members between
	 * ranks low] and ranks [high].
	 *
	 * @param max The highest priveleged member to retrieve.
	 * @param min The lowest priveleged member to retrieve.
	 * @return A reference to the membership query vector.
	 */
	std::vector <std::string>& getMembership(
			const std::string& max, const std::string& min);

	/*!\brief Adds a user to the group by rank. */
	void addMember(const std::string& mogu_userid, const int& rank);

	/*!\brief Adds a user to the group by level. */
	void addMember(const std::string& mogu_userid, const std::string& level);

	int getMemberRank(const std::string& mogu_userid);
};


#endif /* GROUPMANAGER_H_ */
