#ifndef FAQ_TOOLS_HPP
#define FAQ_TOOLS_HPP
#include "FAQRow.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include "jwt-cpp/jwt.h"
#include <crow.h>
/*
 * Utilitary class.
 */
class Tools
{
  public:
    static unsigned int randomUInt()
    {
        // Current timestamp.
        auto now = std::chrono::high_resolution_clock::now();
        auto seed = now.time_since_epoch().count();

        // Use current timestamp as seed for our random generator.
        std::mt19937 generator(seed);

        // Create a uniform distribution limited to unsigned int range.
        std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());

        // Generate random uint.
        return distribution(generator);
    }
    static FAQRow convertToFAQRow(const SQLite::Statement &query)
    {

        FAQRow row;

        row.ROWID = query.getColumn(0);
        row.QUESTION = std::string(query.getColumn(1));
        row.RESPONSE = std::string(query.getColumn(2));
        row.SHOW = (unsigned int)query.getColumn(3) == 1 ? true : false;

        return row;
    }
    static int64_t currentTimestamp(const std::chrono::minutes addedminutes = std::chrono::minutes(0))
    {
        auto currentTime = std::chrono::system_clock::now();
        auto finalTime = currentTime + addedminutes;
        return finalTime.time_since_epoch().count();
    }
    /*
     * Method for JWT token creation.
     * @param iss : issuer.
     * @param scope : token scope.
     * @param aud : token audience.
     * @param pkey : private key.
     */
    static std::string JWTToken(const std::string &iss, const std::string &scope, const std::string &aud,
                                const std::string &pkey)
    {
        auto token = jwt::create()
                         .set_issuer(iss)
                         .set_type("JWT")
                         .set_id("rsa-create-example")
                         .set_issued_now()
                         .set_expires_in(std::chrono::seconds{1800})
                         .set_payload_claim("scope", picojson::value(scope))
                         .set_audience(aud)
                         .sign(jwt::algorithm::rs256("", pkey, "", ""));
        return token;
    }
    /*
     * Conversion from FAQRow to wvalue.
     * @param faq : row.
     * @return wvalue corresponding.*/
    static crow::json::wvalue convertToWValue(const FAQRow &faq)
    {
        crow::json::wvalue outValue;
        outValue["rowid"] = faq.ROWID;
        outValue["question"] = faq.QUESTION;
        outValue["response"] = faq.RESPONSE;
        outValue["show"] = faq.SHOW;
        return outValue;
    }
    /*
     * Conversion from vector to list of wvalue. It uses convertToWValue.
     * @param list : liste to convert
     * @return wvalue containing a vector of unitary wvalues.
     */
    template <typename T> static crow::json::wvalue convertListToWValue(const std::vector<T> &list)
    {
        std::vector<crow::json::wvalue> listValue;
        std::for_each(list.begin(), list.end(),
                      [&listValue](const T &value) { listValue.push_back(convertToWValue(value)); });
        return crow::json::wvalue(crow::json::wvalue::list(listValue));
    }
};
#endif
