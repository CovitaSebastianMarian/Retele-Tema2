#pragma once
#include <array>
#include <string>



const std::array<std::pair<std::string, std::string>, 9> commands = {
    std::make_pair("[VITEZA]", "Am receptionat viteza!"),
    std::make_pair("[ACCIDENT]", "Am inregistrat accidentul!"),
    std::make_pair("[ABONARE_VREME]", "Te-am abonat la vreme!"),
    std::make_pair("[DEZABONARE_VREME]", "Te-am dezabonat de la vreme!"),
    std::make_pair("[ABONARE_SPORT]", "Te-am abonat la sport!"),
    std::make_pair("[DEZABONARE_SPORT]", "Te-am dezabonat de la sport!"),
    std::make_pair("[ABONARE_COMBUSTIBIL]", "Te-am abonat la combustibil!"),
    std::make_pair("[DEZABONARE_COMBUSTIBIL]", "Te-am dezabonat de la combustibil!"),
    std::make_pair("[CHAT]", "Mesaj trimis cu succes!")
};

class Message {
public:
    static std::string decode(const std::string& msg) {
        for(const auto& s : commands) {
            if(msg.rfind(s.first, 0) == 0) {
                std::string rest = msg.substr(s.first.size());
                return s.second;
            }
        }
        return "Comanda nu exista!";
    }
};