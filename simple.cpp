#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cJSON.h>
using namespace std;

class sql_column{

	public:
		string colname;
		string coltype;
		sql_column(string &s , string &r){
			colname = s;
			coltype = r;
		}
};

class sql_table{

	public:
		string tname;
		map<string,sql_column> list;
		sql_table(string& _name){
			tname = _name;
		}

		void insert(string& colName, string& colType ){
			sql_column column(colName,colType);
			std::pair<string,sql_column> _pair(colName,column);
			list.insert(_pair);
		}

		sql_column* getColumnByName(string &columnName){
			std::map<string,sql_column>::iterator position;
			position = list.find(columnName);
			if(position == list.end()){
				//std::assert(0 && "No DB Found");
				return NULL;
			}		
			return &position->second;
		}
};

class sql_schema{

	public:
		string sname;
		map<string,sql_table> list;
		sql_schema(string& _name){
			sname = _name;
		}

		void insert(string& tableName){
			sql_table table(tableName);
			std::pair<string,sql_table> _pair(tableName,table);
			list.insert(_pair);
		}

		sql_table* getTableByName(string &tableName){
			std::map<string,sql_table>::iterator position;
			position = list.find(tableName);
			if(position == list.end()){
				//std::assert(0 && "No DB Found");
				return NULL;
			}
			return &position->second;
		}
};



class sql_database{
	private :
		sql_database(){

		}

	public:

		string dbname;
		map<string,sql_schema> list;

		sql_database(string& _name){
			dbname = _name;
		}
		void insert(string& schemaName){
			sql_schema schema(schemaName);
			std::pair<string,sql_schema> _pair(schemaName,schema);
			list.insert(_pair);
		}

		sql_schema* getSchemaByName(string &schemaName){
			std::map<string,sql_schema>::iterator position;
			position = list.find(schemaName);
			if(position == list.end()){
				//std::assert(0 && "No DB Found");
				return NULL;
			}	
			return &position->second;
		}
};

class sql_Database_Collection{

	private:
		std::map<string,sql_database> list;
	public:

		sql_database* getDatabaseBy(string& dbName)
		{
			std::map<string,sql_database>::iterator position;
			position = list.find(dbName);
			if(position == list.end()){
				//std::assert(0 && "No DB Found");
				return NULL;
			}

			return &position->second;
		}

		void register_database(string dbName){
			sql_database db(dbName);
			std::pair<string,sql_database> _pair(dbName,db);
			list.insert(_pair);

		}


		void print(){

			for(map<string,sql_database>::iterator it = list.begin() ;  it!= list.end() ; it++){
				cout << (*it).first << " " <<(*it).second.dbname << endl;
				string tem1 = (*it).first;
				for(map<string,sql_schema>::iterator sit = (*it).second.list.begin() ;  sit!= (*it).second.list.end() ; sit++){
					cout << (*sit).first <<" "  << (*sit).second.sname << endl;
					string tem2 = (*sit).first;
					for(map<string,sql_table>::iterator tit = (*sit).second.list.begin() ;  tit!= (*sit).second.list.end() ; tit++){
						cout << (*tit).first<< "  " << (*tit).second.tname << endl;
						string tem3 = (*tit).first;
						for(map<string,sql_column>::iterator cit = (*tit).second.list.begin() ;  cit!= (*tit).second.list.end() ; cit++){
							cout << (*cit).first << " " << (*cit).second.colname << " " << (*cit).second.coltype << endl;
						}					
					}	
				}
			}
		}
};


int main(void)
{
	string line,fline;
	ifstream myfile("C:\\Users\\raluka\\Desktop\\json.txt");
	if (myfile){
		while (getline( myfile, line )) 
		{
			fline +=line;
		}
		myfile.close();
	}
	else{
		cout << "No File Found or Could not Open\n";
	} 

	fline.erase(remove_if(fline.begin(), fline.end(), isspace), fline.end());
	if(!fline.empty()){
		cJSON* request_json = cJSON_Parse(fline.c_str());
		cJSON* database = cJSON_GetObjectItem(request_json, "database");	
		//sql_database  *p = new sql_database[r];
		sql_Database_Collection dbCollection;

		while(database->child){
			cJSON *dname = cJSON_GetObjectItem(database->child,"name");
			cJSON *schemas = cJSON_GetObjectItem(database->child,"schemas");
			string s = dname->valuestring;
			sql_database* db = dbCollection.getDatabaseBy(s);
			if( NULL == db ){
				dbCollection.register_database(s);
				db = dbCollection.getDatabaseBy(s);
			}
			while(schemas->child)
			{
				cJSON *sname = cJSON_GetObjectItem(schemas->child, "name");
				cJSON *tables = cJSON_GetObjectItem(schemas->child, "tables");
				string schemName = sname->valuestring;
				sql_schema* db1 = db->getSchemaByName(schemName);
				if( NULL == db1 ){
					db->insert(schemName);
					db1 = db->getSchemaByName(schemName);
				}	
				while(tables->child){
					cJSON *tname = cJSON_GetObjectItem(tables->child, "name");
					cJSON *columns = cJSON_GetObjectItem(tables->child, "columns");
					string tabName = tname->valuestring;
					sql_table* db2 = db1->getTableByName(tabName);
					if( NULL == db2 ){
						db1->insert(tabName);
						db2 = db1->getTableByName(tabName);
					}	
					while(columns->child){
						cJSON *columnName = cJSON_GetObjectItem(columns->child, "name");
						cJSON *columnType = cJSON_GetObjectItem(columns->child, "type");
						string coluName = columnName->valuestring;
						string coluType = columnType->valuestring;
						sql_column* db3 = db2->getColumnByName(coluName);
						if( NULL == db3 ){
							db2->insert(coluName,coluType);
							db3 = db2->getColumnByName(coluName);
						}
						columns->child = columns->child->next;
					}
					tables->child = tables->child->next;
				}
				schemas->child = schemas->child->next;
			}
			database->child = database->child->next;
		}
		dbCollection.print();
	}
	return 0;
}

