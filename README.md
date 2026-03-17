# Tekken Battle DSL Compiler in C++

## How to Compile and Run

To compile:

g++ Tekken.cpp -o tekken
or (recommended for C++11 support):
g++ -std=c++11 Tekken.cpp -o tekken

To run:
./tekken

---

## Project Overview

This project implements a domain-specific language (DSL) for simulating battles inspired by the Tekken video game. The language is embedded in C++ and allows users to define fighters, abilities, and duels using a custom syntax.

The DSL is translated into valid C++ using macros, operator overloading, and helper classes.

---

## Features

### Fighter Definition

Fighters are defined with:
- Name
- Type
- Health Points (HP)

Supported types:
- Rushdown
- Grappler
- Heavy
- Evasive

Example:

CREATE FIGHTER {
    NAME: "King",
    TYPE: "Grappler",
    HP: 150
}

---

### Ability Definition

Abilities define actions that fighters can perform.

Each ability includes:
- A name
- An ACTION block

Example:

CREATE ABILITY {
    NAME: "Head_Smash",
    ACTION: START
        DAMAGE DEFENDER 22
    END
}

---

### Supported Actions

Inside abilities:

- DAMAGE DEFENDER/ATTACKER number
- HEAL DEFENDER/ATTACKER number
- TAG DEFENDER/ATTACKER value

Utility functions:
- GET_HP(...)
- GET_TYPE(...)
- GET_NAME(...)
- IS_OUT_OF_RING(...)

---

### Control Flow

Conditionals:

IF condition DO
    ...
ELSE_IF condition DO
    ...
ELSE
    ...
END

Loops:

FOR 5 ROUNDS DO
    ...
END

Delayed Actions:

AFTER 2 ROUNDS DO
    ...
END

Output:

SHOW ...

---

### Assigning Abilities

Fighters must learn abilities:

DEAR "Lee" LEARN [
    ABILITY_NAME(Give_Autographs)
    ABILITY_NAME(Head_Smash)
]

---

### Type-Based Mechanics

Each fighter type has special behavior:

- Heavy: receives reduced damage  
- Rushdown: deals increased damage  
- Evasive: deals slightly more damage and receives less  
- Grappler: gains bonuses depending on round number  

---

### Duel System

The DUEL command starts the fight.

Flow:
1. Fighters are displayed
2. Players choose fighters
3. Fighters enter the ring
4. Each round, abilities are selected
5. Status updates after each action
6. Fight ends when HP reaches zero

---

## Example Program

#include "Tekken.h"
BEGIN_GAME

CREATE ABILITY {
    NAME:"Give_Autographs",
    ACTION: START
        TAG DEFENDER ---
        AFTER 2 ROUNDS DO
            TAG DEFENDER _
        END
    END
}

CREATE ABILITY {
    NAME: "Head_Smash",
    ACTION: START
        DAMAGE DEFENDER 22
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
]

DEAR "Jack-6" LEARN [
    ABILITY_NAME(Head_Smash)
]

DUEL

END_GAME

---

## Implementation Details

- Uses C++ macros to simulate DSL syntax  
- Uses operator overloading for expressive constructs  
- Uses helper classes and objects  
- Compatible with C++11  
