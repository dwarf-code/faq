#ifndef FAQ_SQLITEDATAACCESS_HPP
#define FAQ_SQLITEDATAACCESS_HPP
#include "FAQRow.hpp"
#include "IDataAccess.hpp"
#include "SQLiteCpp/Database.h"
#include "Tools.hpp"
#include <iostream>
class SQLiteDataAccess : public IDataAccess
{
  public:
    SQLiteDataAccess(const std::string &database)
    {
        mDatabase = database;

        SQLite::Database db(mDatabase, SQLite::OPEN_READWRITE | SQLite::OPEN_NOMUTEX | SQLite::OPEN_CREATE);
        db.exec("CREATE TABLE IF NOT EXISTS FAQ(QUESTION TEXT, RESPONSE TEXT, SHOW INT)");
    }
    virtual bool createQuestion(const std::string &question) override
    {
        std::cout << "CREATE "
                  << " Question : " << question << std::endl;
        try
        {
            // Open a database file
            SQLite::Database db(mDatabase, SQLite::OPEN_READWRITE | SQLite::OPEN_NOMUTEX);

            SQLite::Statement query(db, "INSERT INTO FAQ VALUES (?,NULL,0)");

            query.bind(1, question);
            query.exec();
            return true;
        }
        catch (std::exception &e)
        {
            std::cout << "exception: " << e.what() << std::endl;
        }
        return false;
    }
    /* Method for getting all validated (shown) questions.
     * @return optional with all questions/answers
     */
    virtual std::optional<std::vector<FAQRow>> getAllValidated() override
    {
        return fetchAndMapResults("SELECT ROWID,QUESTION,RESPONSE,SHOW "
                                  "FROM FAQ WHERE SHOW = 1");
    }
    virtual bool deleteQuestion(unsigned int rowId) override
    {
        std::cout << "DELETE" << std::endl;
        try
        {
            // Open a database file
            SQLite::Database db(mDatabase, SQLite::OPEN_READWRITE | SQLite::OPEN_NOMUTEX);

            // Compile a SQL query, containing one parameter (index 1)
            SQLite::Statement query(db, "DELETE FROM FAQ WHERE ROWID=(?)");

            query.bind(1, rowId);
            query.exec();
            return true;
        }
        catch (std::exception &e)
        {
            std::cout << "exception: " << e.what() << std::endl;
        }
        return false;
    }

    virtual bool updateQuestion(unsigned int rowId, const std::string &question, const std::string &answer,
                                bool show) override
    {
        std::cout << "UPDATE " << rowId << " Reponse : " << answer << " show : " << show << std::endl;
        try
        {
            // Open a database file
            SQLite::Database db(mDatabase, SQLite::OPEN_READWRITE | SQLite::OPEN_NOMUTEX);

            SQLite::Statement query(db, "UPDATE FAQ SET QUESTION=?, RESPONSE=?, SHOW=? WHERE ROWID=?");

            query.bind(1, question);
            query.bind(2, answer);
            query.bind(3, show ? 1 : 0);
            query.bind(4, rowId);
            query.exec();
            return true;
        }
        catch (std::exception &e)
        {
            std::cout << "exception: " << e.what() << std::endl;
        }
        return false;
    }

    virtual std::optional<std::vector<FAQRow>> getAll() override
    {
        return fetchAndMapResults("SELECT QUESTION,RESPONSE,SHOW FROM FAQ");
    }
    virtual std::optional<FAQRow> getOne(unsigned int rowid) override
    {
        try
        {
            // Open a database file
            SQLite::Database db(mDatabase);

            // Compile a SQL query, containing one parameter (index 1)
            SQLite::Statement query(db, "SELECT ROWID,QUESTION,RESPONSE,SHOW FROM FAQ WHERE ROWID = ?");
            std::cout << query.getQuery() << std::endl;
            query.bind(1, rowid);

            while (query.executeStep())
            {
                return {Tools::convertToFAQRow(query)};
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Cannot retrieve one question : " << rowid << " reason : " << e.what() << std::endl;
        }
        return std::nullopt;
    }

  private:
    std::optional<std::vector<FAQRow>> fetchAndMapResults(const std::string &request)
    {

        try
        {
            // Open a database file
            SQLite::Database db(mDatabase);

            // Compile a SQL query, containing one parameter (index 1)
            SQLite::Statement query(db, request);

            std::vector<FAQRow> returnValue;
            // Loop to execute the query step by step, to get rows of result
            while (query.executeStep())
            {
                returnValue.push_back(Tools::convertToFAQRow(query));
            }
            return {returnValue};
        }
        catch (std::exception &e)
        {
            std::cout << "exception: " << e.what() << std::endl;
        }
        return std::nullopt;
    }
    std::string mDatabase;
};
#endif
