#include "Tekken.h"

BEGIN_GAME

/*//basic damage (test DAMAGE + type modifier effects)
CREATE ABILITY {
    NAME: "Jab",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " uses Jab on " << GET_NAME(DEFENDER) << "\n";
        DAMAGE DEFENDER 10
    END
}

//test HEAL
CREATE ABILITY {
    NAME: "Small_Heal",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " uses Small_Heal\n";
        HEAL ATTACKER 12
    END
}*/

CREATE ABILITIES [
    ABILITY {
        NAME: "Jab",
        ACTION: START
            SHOW ">> " << GET_NAME(ATTACKER) << " uses Jab on " << GET_NAME(DEFENDER) << "\n";
            DAMAGE DEFENDER 10
        END
    },
    ABILITY {
        NAME: "Small_Heal",
        ACTION: START
            SHOW ">> " << GET_NAME(ATTACKER) << " uses Small_Heal\n";
            HEAL ATTACKER 12
        END
    }
]

//ring-out now + return after 2 rounds (tests TAG,---α,AFTER)
CREATE ABILITY {
    NAME: "Ring_Out_And_Return",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " ring-outs " << GET_NAME(DEFENDER) << "\n";
        TAG DEFENDER ---α

        SHOW "   (scheduled) " << GET_NAME(DEFENDER) << " will return after 2 rounds\n";

        AFTER 2 ROUNDS DO
            SHOW ">> (AFTER) " << GET_NAME(DEFENDER) << " re-enters the ring!\n";
            TAG DEFENDER _
        ENDAFTER
    END
}

//damage over time for 3 rounds (tests FOR scheduling)
CREATE ABILITY {
    NAME: "Poison_3",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " applies Poison_3 to " << GET_NAME(DEFENDER) << "\n";
        FOR 3 ROUNDS DO
            SHOW ">> (FOR) Poison ticks for 6 on " << GET_NAME(DEFENDER) << "\n";
            DAMAGE DEFENDER 6
        ENDFOR
    END
}

//conditional logic using GET_HP + AND/OR/NOT + ELSE_IF/ELSE
CREATE ABILITY {
    NAME: "Smart_Move",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " uses Smart_Move\n";

        IF ( AND(GET_HP(ATTACKER) <= 35, NOT(IS_OUT_OF_RING(ATTACKER))) ) DO_IF
            SHOW "   IF branch: low HP -> heal\n";
            HEAL ATTACKER 25

        ELSE_IF ( OR(GET_HP(DEFENDER) <= 20, IS_OUT_OF_RING(DEFENDER)) ) DO_IF
            SHOW "   ELSE_IF branch: defender weak or out -> heavy hit\n";
            DAMAGE DEFENDER 28

        ELSE
            SHOW "   ELSE branch: normal hit\n";
            DAMAGE DEFENDER 14
        ENDIF
    END
}

//type-based branching using GET_TYPE (tests GET_TYPE + IF chain)
CREATE ABILITY {
    NAME: "Type_Checker",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " uses Type_Checker vs " << GET_NAME(DEFENDER) << "\n";
        SHOW "   Attacker type=" << GET_TYPE(ATTACKER) << ", Defender type=" << GET_TYPE(DEFENDER) << "\n";

        IF (GET_TYPE(ATTACKER) == std::string("Evasive")) DO_IF
            SHOW "   Attacker is Evasive -> quick poke\n";
            DAMAGE DEFENDER 9

        ELSE_IF (GET_TYPE(ATTACKER) == std::string("Grappler")) DO_IF
            SHOW "   Attacker is Grappler -> big slam\n";
            DAMAGE DEFENDER 18

        ELSE
            SHOW "   Other type -> standard hit\n";
            DAMAGE DEFENDER 12
        ENDIF
    END
}

//ring-out + DOT + heal, and uses AFTER + FOR together
CREATE ABILITY {
    NAME: "All_In_One",
    ACTION: START
        SHOW ">> " << GET_NAME(ATTACKER) << " uses All_In_One\n";

        DAMAGE DEFENDER 8
        HEAL ATTACKER 8

        TAG DEFENDER ---α

        AFTER 1 ROUNDS DO
            SHOW ">> (AFTER) Defender returns from All_In_One\n";
            TAG DEFENDER _
        ENDAFTER

        FOR 2 ROUNDS DO
            SHOW ">> (FOR) All_In_One follow-up damage\n";
            DAMAGE DEFENDER 7
        ENDFOR
    END
}

//pure SHOW demo (tests SHOW streaming)
CREATE ABILITY {
    NAME: "Show_Status",
    ACTION: START
        SHOW ">> STATUS\n";
        SHOW "   Attacker: " << GET_NAME(ATTACKER) << " HP=" << GET_HP(ATTACKER)
             << " Type=" << GET_TYPE(ATTACKER)
             << " OutOfRing=" << (IS_OUT_OF_RING(ATTACKER) ? "YES" : "NO") << "\n";
        SHOW "   Defender: " << GET_NAME(DEFENDER) << " HP=" << GET_HP(DEFENDER)
             << " Type=" << GET_TYPE(DEFENDER)
             << " OutOfRing=" << (IS_OUT_OF_RING(DEFENDER) ? "YES" : "NO") << "\n";
    END
}

CREATE FIGHTERS [
    FIGHTER {
        NAME: "Ryu",
        TYPE: "Rushdown",
        HP: 100
    },
    FIGHTER {
        NAME: "Golem",
        TYPE: "Heavy",
        HP: 110
    }
]

CREATE FIGHTER {
    NAME: "Shade",
    TYPE: "Evasive",
    HP: 95
}

CREATE FIGHTER {
    NAME: "Titan",
    TYPE: "Grappler",
    HP: 105
}

DEAR "Ryu" LEARN [
    ABILITY_NAME(Jab)
    ABILITY_NAME(Small_Heal)
    ABILITY_NAME(Ring_Out_And_Return)
    ABILITY_NAME(Poison_3)
    ABILITY_NAME(Smart_Move)
    ABILITY_NAME(Type_Checker)
    ABILITY_NAME(All_In_One)
    ABILITY_NAME(Show_Status)
]

DEAR "Golem" LEARN [
    ABILITY_NAME(Jab)
    ABILITY_NAME(Small_Heal)
    ABILITY_NAME(Ring_Out_And_Return)
    ABILITY_NAME(Poison_3)
    ABILITY_NAME(Smart_Move)
    ABILITY_NAME(Type_Checker)
    ABILITY_NAME(All_In_One)
    ABILITY_NAME(Show_Status)
]

DEAR "Shade" LEARN [
    ABILITY_NAME(Jab)
    ABILITY_NAME(Small_Heal)
    ABILITY_NAME(Ring_Out_And_Return)
    ABILITY_NAME(Poison_3)
    ABILITY_NAME(Smart_Move)
    ABILITY_NAME(Type_Checker)
    ABILITY_NAME(All_In_One)
    ABILITY_NAME(Show_Status)
]

DEAR "Titan" LEARN [
    ABILITY_NAME(Jab)
    ABILITY_NAME(Small_Heal)
    ABILITY_NAME(Ring_Out_And_Return)
    ABILITY_NAME(Poison_3)
    ABILITY_NAME(Smart_Move)
    ABILITY_NAME(Type_Checker)
    ABILITY_NAME(All_In_One)
    ABILITY_NAME(Show_Status)
]

DUEL

END_GAME
