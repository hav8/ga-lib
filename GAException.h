/****************************************************************************
    \cpptex
    \cppfilename{GAException.h}

    В файле "<GAException.h"> содержится описание класса исключения
    {\cppprogram exception} для всех классов используемых в библиотеке.

****************************************************************************/
#ifndef GAEXCEPTION_H
#define GAEXCEPTION_H
//---------------------------------------------------------------------------
// #define GA_DEBUG
//---------------------------------------------------------------------------
#ifndef __FUNC__
#define __FUNC__ "GALib"
#endif  // __FUNC__
//---------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    /// Исключения \cppclassdef{exception}
    //---------------------------------------------------------------------------
    class exception : public std::runtime_error
    {
        std::string function;
    public:
        exception(const std::string &func, const std::string &mess);
        std::ostream &put_message_to_stream(std::ostream &out) const;
    };
    //---------------------------------------------------------------------------
    inline exception::exception(const std::string &func, const std::string &mess) :
        std::runtime_error(mess),
        function          (func)
    {
    }
    //---------------------------------------------------------------------------
    inline std::ostream &exception::put_message_to_stream(std::ostream &out) const
    {
        return out << "Error in " << function << ": " << what();
    }
    //---------------------------------------------------------------------------
} // namespace ga
//---------------------------------------------------------------------------
inline std::ostream &operator<<(std::ostream &out, const ga::exception &exception)
{
    return exception.put_message_to_stream(out);
}
//---------------------------------------------------------------------------
#endif // GAEXEPTION_H