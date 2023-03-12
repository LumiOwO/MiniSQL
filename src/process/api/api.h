#ifndef API_H_
#define API_H_

#include <QString>

class MainWindow;

class API
{
// Singleton
private:
	MainWindow *window;
private:
	API(MainWindow* window):
		window(window) {}
	static API *instance;
public:
	static API *getInstance(MainWindow* window = nullptr);
	static void free();

	// exec SQL Statement
	void execSQL(const QString& codeText);
	// store all pages to file
	void storePages();
	// get all table names in the catalog
	QStringList getAllTables() const;
	// get all attribute names of a table
	QStringList getAttrNames(QString tableName) const;
	// get the header of view table
	QStringList getViewHeader() const;
	// get the records to show
	QVector<QStringList> getViewRecords() const;
	// get all index info to show
	QVector<QStringList> getAllIndexInfo() const;
};


#endif
