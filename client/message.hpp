#pragma once

#include <string>


class Message {
public:
    static std::string Viteza(const std::string& viteza) {
        return "[VITEZA] " + viteza;
    }

    static std::string Strada(const std::string& str) {
        return "[STRADA] " + str;
    }

    static std::string Accident(const std::string& locatie) {
        return "[ACCIDENT] " + locatie;
    }

    static std::string Trafic(const std::string& locatie) {
        return "[TRAFIC] " + locatie;
    }

    static std::string AbonareVreme() {
        return "[ABONARE_VREME]";
    }

    static std::string DezabonareVreme() {
        return "[DEZABONARE_VREME]";
    }

    static std::string AbonareSport() {
        return "[ABONARE_SPORT]";
    }

    static std::string DezabonareSport() {
        return "[DEZABONARE_SPORT]";
    }

    static std::string AbonareCombustibil() {
        return "[ABONARE_COMBUSTIBIL]";
    }

    static std::string DezabonareCombustibil() {
        return "[DEZABONARE_COMBUSTIBIL]";
    }

    static std::string Chat(const std::string& mesaj) {
        return "[CHAT] " + mesaj;
    }
};