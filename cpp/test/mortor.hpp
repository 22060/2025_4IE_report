#ifndef E9535799_05DA_4DB0_9C94_74CDE9955CA2
#define E9535799_05DA_4DB0_9C94_74CDE9955CA2
#include <iostream>

class Mortors
{
public:
    int pwm;
    bool dir;
    Mortors(int pwm, bool dir) : pwm(pwm), dir(dir) {}
    Mortors() : pwm(0), dir(false) {}
    Mortors(const Mortors &other) : pwm(other.pwm), dir(other.dir) {}
    Mortors &operator=(const Mortors &other)
    {
        if (this != &other)
        {
            pwm = other.pwm;
            dir = other.dir;
        }
        return *this;
    }
    void setPwm(int newPwm)
    {
        pwm = newPwm;
    }
    void setDir(bool newDir)
    {
        dir = newDir;
    }
    int getPwm() const
    {
        return pwm;
    }
    bool getDir() const
    {
        return dir;
    }
    void print() const
    {
        std::cout << "Pwm: " << pwm << ", Dir: " << (dir ? "true" : "false") << std::endl;
    }
    bool operator==(const Mortors &other) const
    {
        return (pwm == other.pwm && dir == other.dir);
    }
    bool operator!=(const Mortors &other) const
    {
        return !(*this == other);
    }
}; // ← セミコロンを追加

inline std::ostream &
operator<<(std::ostream &os, const Mortors &m)
{
    os << "Mortors(" << m.getPwm() << ", " << std::boolalpha << m.getDir() << ")";
    return os;
}

#endif /* E9535799_05DA_4DB0_9C94_74CDE9955CA2 */