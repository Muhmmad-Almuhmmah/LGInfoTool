#ifndef PACKSTRUCTURE_H
#define PACKSTRUCTURE_H
/**
 * Developed By Muhammad Almuhmmah
 * Year : 2021
 * Freeware File
 *
Smart Pack Structure For QC Diag And LG LAF Commands
 - High flexibility in use
 - Easy and expandable without the usual limits
 - It can easily be expanded to support other protocols
 - It automatically generates the value CRC X25
 - Flexibility in arguments is limitless
 C++ Qt
 **/
#include <QtCore>
// see -> https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>
#include <iostream>

#define QD(x) qDebug() <<x;
#define getMemoryDataHex(x) QByteArray((const char*)&x,sizeof(x)).toHex()

#define \
    Int2Hex(xnum)({\
    QString str = QString::number((qlonglong)xnum,16);\
    str;\
    })


#define \
    GetUint64(stringNumber)({\
    std::string s = stringNumber;\
    uint64_t newUint = std::stoull(s, nullptr, 16);\
    newUint;\
    })

#define \
    GetUint32(stringNumber)({\
    std::string s = stringNumber;\
    uint32_t newUint = std::stoul(s, nullptr, 16);\
    newUint;\
    })

#define Buffer2intger(buffer,start,end) buffer.mid(start,end).toHex().toInt(NULL,16)

template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
            (
            #ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
            #else
                nullptr,
            #endif
                std::free
                );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

typedef struct __arg_item__{
    __arg_item__(const QVariant &variant,const QString &name){
        Variant=variant;
        Name=name;
    }
    QVariant Variant;
    QString Name;
}ArgItem;

template<typename ... Args>
static QByteArray PackStructureLaf(const Args&... args) // At least one argument
{
    QD(endl<<endl<<"------------------                    PackLafStructure 2               ------------------");
    QList<ArgItem> lv;
    int sink[] = { 0, ((void)(lv.append(ArgItem(args,QString::fromStdString(type_name<decltype(args)>())))), 0)... };(void) sink;
    bool bls=1;
    QVariant vArg;
    QByteArray output;
    int type=0;
    for(int i=0;i<lv.count();i++)
    {
        ArgItem item=lv.at(i);
        vArg=item.Variant;
        //        qDebug() <<item.Name<<vArg<<output;
        type=(int)(QMetaType::Type)vArg.type();
        if(type==QMetaType::Type::UInt or type==QMetaType::Type::Double or type==QVariant::Int or type==QVariant::LongLong or type==QVariant::ULongLong){
            if(item.Name=="unsigned char const&"){
                unsigned char const carVl=vArg.toChar().toLatin1();
                output.append(getMemoryDataHex(carVl));
            }else if(item.Name=="short const&" or item.Name=="unsigned short const&"){
                short shortVl=vArg.toInt();
                output.append(getMemoryDataHex(shortVl));
            }else if(type==QMetaType::Type::Double){
                double NumberVl=vArg.toDouble();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::Int){
                int NumberVl=vArg.toInt();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::UInt){
                uint NumberVl=vArg.toUInt();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::LongLong){
                qlonglong NumberVl=vArg.toLongLong();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::ULongLong){
                qulonglong NumberVl=vArg.toULongLong();
                output.append(getMemoryDataHex(NumberVl));
            }
        }else if(type==QVariant::String){
            output.append(vArg.toString());
        }else if(type==QVariant::Char)
            output.append(vArg.toChar());
        else if(type==QVariant::ByteArray)
            output.append(vArg.toByteArray());
    }
   // calculate crc x25
    if(output.length()>=32){
        QByteArray ddd=QByteArray::fromHex(output);
        quint16 crc=qChecksum(ddd.data(),ddd.length());
        output.clear();
        output=ddd.mid(0,24);
        output.append((char*)&crc,sizeof(crc));
        output.append(QByteArray().fill(0x0,2));
        output.append(ddd.mid(28));
        output=output.toHex();
    }
    if(output.length()>=300){
        bls=0;
    }
    if(bls)
        qDebug() <<"output[HEX]"<<output.toUpper();
    output=QByteArray::fromHex(output);
    if(bls)
        qDebug() <<"output"<<output;
    return output;
}
template<typename ... Args>
static QByteArray PackStructureQC(const Args&... args) // At least one argument
{
    QD(endl<<endl<<"------------------                    PackStructureQC                ------------------");
    QList<ArgItem> lv;
    int sink[] = { 0, ((void)(lv.append(ArgItem(args,QString::fromStdString(type_name<decltype(args)>())))), 0)... };(void) sink;
    bool bls=1;
    QVariant vArg;
    QByteArray output;
    int type=0;
    for(int i=0;i<lv.count();i++)
    {
        ArgItem item=lv.at(i);
        vArg=item.Variant;
        type=(int)(QMetaType::Type)vArg.type();
        if(type==QMetaType::Type::UInt or type==QMetaType::Type::Double or type==QVariant::Int or type==QVariant::LongLong or type==QVariant::ULongLong){
            if(item.Name=="unsigned char const&"){
                unsigned char const carVl=vArg.toChar().toLatin1();
                output.append(getMemoryDataHex(carVl));
            }else if(item.Name=="short const&" or item.Name=="unsigned short const&"){
                short shortVl=vArg.toInt();
                output.append(getMemoryDataHex(shortVl));
            }else if(type==QMetaType::Type::Double){
                double NumberVl=vArg.toDouble();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::Int){
                int NumberVl=vArg.toInt();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::UInt){
                uint NumberVl=vArg.toUInt();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::LongLong){
                qlonglong NumberVl=vArg.toLongLong();
                output.append(getMemoryDataHex(NumberVl));
            }else if(type==QMetaType::Type::ULongLong){
                qulonglong NumberVl=vArg.toULongLong();
                output.append(getMemoryDataHex(NumberVl));
            }
        }else if(type==QVariant::String){
            output.append(vArg.toString());
        }else if(type==QVariant::Char)
            output.append(vArg.toChar());
        else if(type==QVariant::ByteArray)
            output.append(vArg.toByteArray());
    }
    // calculate crc x25
    if(output.length()>=2){
        QByteArray ddd=QByteArray::fromHex(output);
        quint16 crc=qChecksum(ddd.data(),ddd.length());
        output.clear();
        output=ddd;
        output.append((char*)&crc,sizeof(crc));
        output.append(0x7E);
        output=output.toHex();
    }
    qDebug() <<"output[HEX] QC "<<output.toUpper();
    output=QByteArray::fromHex(output);
    return output;
}

template<typename ... Args>
static QByteArray PackStructure(const Args&... args) // At least one argument
{
    QList<QVariant> lv;
    int sink[] = { 0, ((void)(lv.append(args)), 0)... };(void) sink;
    QVariant vArg;
    QByteArray output;
    int type=0;
    for(int i=0;i<lv.count();i++)
    {
        vArg=lv.at(i);
        qDebug() <<vArg;
        type=(int)(QMetaType::Type)vArg.type();
        if(type==QMetaType::Type::UInt or type==QMetaType::Type::Double or type==QVariant::Int or type==QVariant::LongLong or type==QVariant::ULongLong){
            if(type==QMetaType::Type::Double)
                output.append(Int2Hex(vArg.toDouble()));
            else if(type==QMetaType::Type::Int)
                output.append(Int2Hex(vArg.toInt()));
            else if(type==QMetaType::Type::UInt)
                output.append(Int2Hex(vArg.toUInt()));
            else if(type==QMetaType::Type::LongLong)
                output.append(Int2Hex(vArg.toLongLong()));
            else if(type==QMetaType::Type::ULongLong)
                output.append(Int2Hex(vArg.toULongLong()));
        }else if(type==QVariant::String)
            output.append(vArg.toString());
        else if(type==QVariant::Char)
            output.append(vArg.toChar());
        else if(type==QVariant::ByteArray)
            output.append(vArg.toByteArray());
    }
    qDebug() <<"output[HEX]"<<output.toUpper();
    output=QByteArray::fromHex(output);
    qDebug() <<"output"<<output;
    return output;
}

#define QC_FILL(len)                       QByteArray().fill('0',len*2)
#define QC_STANDARD_FILL(len)              QByteArray().fill('0',len)

inline void Filter(QByteArray &data)
{
    if(data.isEmpty())
        return;
    for(int i=0;i<data.count();i++)
    {
        if(data.at(i)==0x00)
            data.remove(i,1);
    }
    for(int i=data.count()-1;i>=0;i--)
    {
        if(data.at(i)==0x00)
            data.remove(i,1);
    }
}

inline QByteArray Filter2(QByteArray data)
{
    if(data.isEmpty())
        return data;
    for(int i=0;i<data.count();i++)
    {
        if(data.at(i)==0x00)
            data.remove(i,1);
    }
    for(int i=data.count()-1;i>=0;i--)
    {
        if(data.at(i)==0x00)
            data.remove(i,1);
    }
    return data;
}
#endif // PACKSTRUCTURE_H
