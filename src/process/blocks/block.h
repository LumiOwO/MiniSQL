#ifndef BLOCK_H_
#define BLOCK_H_

#include <QDebug>
#include <QString>

#define Byte char
#define intRef int&
#define uintRef unsigned int&
#define charRef char&
#define CString char*
#define CStringRef CString&

#define pToData Byte*

// base class
class Block
{
public:
	class Header
	{
	public:
		// reference to data zone
		const CStringRef name;
		charRef id;
		intRef No;
		uintRef validBytes;

		Header(const CStringRef startAddr);
		uint bytes() const { return STRLEN_MAX + sizeof(id) + sizeof(No) + sizeof(validBytes); }
	};
public:
	static const int BLOCK_SIZE = 1 << 12;	// 4kb
	static const int STRLEN_MAX = 20;
protected:
	// data zone
	const pToData dataZone;
private:
	// control signal
	bool dirty;
	bool pinned;
	// header
	Block::Header header;
public:
	Block(char id, int No, QString name);
	virtual ~Block();

	// getter
	char getID() const { return header.id; }
	int getNo() const { return header.No; }
	QString getName() const { return header.name; }
	const pToData getDataZone() const { return dataZone; }
	bool isdirty() const { return dirty; }
	bool isPinned() const { return pinned; }
	// setter
	void setNo(int No) { header.No = No; }
	void setDirty(bool dirty) { this->dirty = dirty; }
	void setPinned(bool pinned) { this->pinned = pinned; }
	void modifyValidBytes(int delta) {
		delta > 0? header.validBytes += uint(delta): header.validBytes -= uint(-delta);
	}

	// get offset
	uint offsetToBegin() const { return header.bytes(); }
	uint offsetToEnd() const { return header.validBytes; }

	// debug
	friend QDebug operator<<(QDebug debug, const Block& block);
	friend QDebug operator<<(QDebug debug, const Block* block);
};

#endif
