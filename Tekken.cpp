#include "Tekken.h"

BEGIN_GAME

CREATE ABILITY {
    NAME: "Give_Autographs",
    ACTION: START
        //Βγάζει τον αντίπαλο fighter από το ring
        TAG DEFENDER ---α

        // Μετά απο 2 γύρους ξαναβάζει τον αντίπαλο(DEFENDER)
        AFTER 2 ROUNDS DO
            TAG DEFENDER _
        END
    END
}

CREATE ABILITY {
    NAME: "Bleeding_Bite",
    ACTION: START
        // Για 5 γύρους κάνει 8 damage στον αντίπαλο (DEFENDER)
        FOR 5 ROUNDS DO
            DAMAGE DEFENDER 8
        END
    END
}

CREATE ABILITY {
    NAME: "Head_Smash",
    ACTION: START
        // Κάνει 22 damage στον αντίπαλο (DEFENDER)
        DAMAGE DEFENDER 22
    END
}

CREATE ABILITY {
    NAME: "Catch_A_Break",
    ACTION: START
        // Κάνει heal στον εαυτό του (ATTACKER)
        HEAL ATTACKER 30
    END
}

CREATE FIGHTER {
    NAME: "Lee",
    TYPE: "Rushdown",
    HP: 100
}

CREATE FIGHTER {
    NAME: "Jack-6",
    TYPE: "Heavy",
    HP: 90
}

DEAR "Lee" LEARN [
    ABILITY_NAME(Give_Autographs)
    ABILITY_NAME(Head_Smash)
    ABILITY_NAME(Catch_A_Break)
    ABILITY_NAME(Bleeding_Bite)
]

DEAR "Jack-6" LEARN [
    ABILITY_NAME(Head_Smash)
    ABILITY_NAME(Catch_A_Break)
    ABILITY_NAME(Bleeding_Bite)
]

DUEL

END_GAME
