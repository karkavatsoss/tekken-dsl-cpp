#ifndef TEKKEN_H
#define TEKKEN_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <sstream>

class Fighter;
class Ability;
class Game;

Game* _game = nullptr;
Fighter* _temp_fighter = nullptr;
Ability* _temp_ability = nullptr;
Fighter* _pending_fighter = nullptr;
Ability* _pending_ability = nullptr;

class Fighter {
public:
    std::string name;
    std::string type;
    int hp;
    int maxHp;
    bool inRing;
    std::vector<std::string> abilityNames;

    Fighter() : hp(100), maxHp(100), inRing(true) {}

    void print() const {
        std::cout << "###########################" << std::endl;
        std::cout << "Name: " << name << std::endl;
        std::cout << "HP: " << hp << std::endl;
        std::cout << "Type: " << type << std::endl;
        std::cout << "###########################" << std::endl;
    }

    bool isDefeated() const { return hp <= 0; }
};

class Ability {
public:
    std::string name;
    std::function<void(Fighter*, Fighter*)> action;
    Ability() {}
    void execute(Fighter* a, Fighter* d) { if (action) action(a, d); }
};

struct DelayedAction { int round; Fighter* atk; Fighter* def; std::function<void(Fighter*, Fighter*)> act; };
struct RepeatingAction { int startR, endR; Fighter* atk; Fighter* def; std::function<void(Fighter*, Fighter*)> act; };

class Game {
public:
    std::map<std::string, Fighter*> fighters;
    std::map<std::string, Ability*> abilities;
    std::vector<DelayedAction> delayed;
    std::vector<RepeatingAction> repeating;
    int currentRound = 0;
    Fighter *p1 = nullptr, *p2 = nullptr;

    ~Game() {
        for (auto& p : fighters) delete p.second;
        for (auto& p : abilities) delete p.second;
    }

    void addFighter(Fighter* f) { fighters[f->name] = f; }
    void addAbility(Ability* a) { abilities[a->name] = a; }
    Fighter* getFighter(const std::string& n) { auto it = fighters.find(n); return it != fighters.end() ? it->second : nullptr; }
    Ability* getAbility(const std::string& n) { auto it = abilities.find(n); return it != abilities.end() ? it->second : nullptr; }

    void teachAbility(const std::string& f, const std::string& a) {
        Fighter* ff = getFighter(f);
        if (ff && abilities.count(a)) ff->abilityNames.push_back(a);
    }

    void scheduleAfter(int r, Fighter* a, Fighter* d, std::function<void(Fighter*, Fighter*)> fn) {
        delayed.push_back({currentRound + r, a, d, fn});
    }
    void scheduleFor(int r, Fighter* a, Fighter* d, std::function<void(Fighter*, Fighter*)> fn) {
        repeating.push_back({currentRound + 1, currentRound + r, a, d, fn});
    }

    void processScheduled() {
        for (auto it = delayed.begin(); it != delayed.end();) {
            if (it->round == currentRound) { it->act(it->atk, it->def); it = delayed.erase(it); }
            else ++it;
        }
        for (auto& r : repeating) {
            if (currentRound >= r.startR && currentRound <= r.endR) r.act(r.atk, r.def);
        }
        repeating.erase(std::remove_if(repeating.begin(), repeating.end(),
            [this](const RepeatingAction& r) { return currentRound > r.endR; }), repeating.end());
    }

    double modifier(Fighter* atk, Fighter* def) {
        double m = 1.0;
        if (atk->type == "Rushdown") m *= (def->type == "Grappler") ? 1.20 : 1.15;
        if (atk->type == "Evasive") m *= 1.07;
        if (atk->type == "Grappler" && currentRound % 2 == 1) m *= 1.07;
        if (def->type == "Heavy") m *= (atk->type == "Evasive") ? 0.70 : 0.80;
        if (def->type == "Evasive") m *= 0.93;
        return m;
    }

    void doDamage(Fighter* atk, Fighter* tgt, int base) {
        int dmg = (int)std::round(base * modifier(atk, tgt));
        tgt->hp = std::max(0, tgt->hp - dmg);
    }
    void doHeal(Fighter* tgt, int amt) { tgt->hp = std::min(tgt->maxHp, tgt->hp + amt); }

    void startDuel() {
        std::cout << "----------------------------FIGHTER THE GAME-------------------------------" << std::endl;
        std::cout << std::endl << "Player1 select fighter:" << std::endl;
        std::cout << "------------------------" << std::endl;
        for (auto& p : fighters) std::cout << p.first << std::endl;
        std::cout << "------------------------" << std::endl;
        std::string c; std::getline(std::cin, c);
        Fighter* p1Base = getFighter(c); if (!p1Base) { std::cout << "Invalid!" << std::endl; return; }
        std::cout << std::endl << "Player2 select fighter:" << std::endl;
        std::cout << "------------------------" << std::endl;
        for (auto& p : fighters) std::cout << p.first << std::endl;
        std::cout << "------------------------" << std::endl;
        std::getline(std::cin, c);
        Fighter* p2Base = getFighter(c); if (!p2Base) { std::cout << "Invalid!" << std::endl; return; }

        p1 = new Fighter(*p1Base);
        p2 = new Fighter(*p2Base);
        auto cleanup = [&]() {
            delete p1;
            delete p2;
            p1 = nullptr;
            p2 = nullptr;
        };

        p1->hp = p1->maxHp; p2->hp = p2->maxHp;
        p1->inRing = p2->inRing = true;

        while (!p1->isDefeated() && !p2->isDefeated()) {
            currentRound++;
            std::cout << std::endl << "------------------------------------------------------------------------" << std::endl;
            std::cout << "Round " << currentRound << std::endl;
            std::cout << "------------------------------------------------------------------------" << std::endl;
            if (currentRound % 2 == 0) {
                if (p1->type == "Grappler") doHeal(p1, (int)(p1->maxHp * 0.05));
                if (p2->type == "Grappler") doHeal(p2, (int)(p2->maxHp * 0.05));
            }
            processScheduled();
            std::cout << std::endl;
            if (p1->inRing) {
                std::cout << p1->name << "(Player1) select ability:" << std::endl;
                std::cout << "------------------------" << std::endl;
                for (auto& a : p1->abilityNames) std::cout << a << std::endl;
                std::cout << "------------------------" << std::endl;
                std::getline(std::cin, c);
                Ability* ab = getAbility(c); if (ab) ab->execute(p1, p2);
            } else std::cout << p1->name << "(Player1) has not a fighter that can enter the ring so he can't cast an ability." << std::endl;
            std::cout << std::endl; p1->print(); std::cout << std::endl; p2->print();
            if (p2->isDefeated()) { std::cout << std::endl << "========================================" << std::endl << p1->name << " WINS!" << std::endl << "========================================" << std::endl; cleanup(); return; }
            std::cout << std::endl;
            if (p2->inRing) {
                std::cout << p2->name << "(Player2) select ability:" << std::endl;
                std::cout << "------------------------" << std::endl;
                for (auto& a : p2->abilityNames) std::cout << a << std::endl;
                std::cout << "------------------------" << std::endl;
                std::getline(std::cin, c);
                Ability* ab = getAbility(c); if (ab) ab->execute(p2, p1);
            } else std::cout << p2->name << "(Player2) has not a fighter that can enter the ring so he can't cast an ability." << std::endl;
            std::cout << std::endl; p1->print(); std::cout << std::endl; p2->print();
            if (p1->isDefeated()) { std::cout << std::endl << "========================================" << std::endl << p2->name << " WINS!" << std::endl << "========================================" << std::endl; cleanup(); return; }
        }
        cleanup();
    }
};

struct _Sink {
    _Sink() {}
    _Sink(int) {}
    _Sink(const std::string&) {}
    _Sink(const char*) {}
    _Sink(double) {}
    _Sink(bool) {}
    _Sink(const std::function<void(Fighter*, Fighter*)>&) {}
    template<typename T> _Sink(const T&) {}
    template<typename T> _Sink operator,(const T&) const { return _Sink(); }
};
static _Sink _sink;

inline Fighter* _ensure_fighter() {
    if (_temp_fighter) return _temp_fighter;
    if (!_pending_fighter) _pending_fighter = new Fighter();
    return _pending_fighter;
}

inline Ability* _ensure_ability() {
    if (_temp_ability) return _temp_ability;
    if (!_pending_ability) _pending_ability = new Ability();
    return _pending_ability;
}

struct _NameSlot {
    _NameSlot& operator=(const char* n) {
        if (_temp_fighter) { _temp_fighter->name = n; return *this; }
        if (_temp_ability) { _temp_ability->name = n; return *this; }
        if (!_pending_fighter) _pending_fighter = new Fighter();
        if (!_pending_ability) _pending_ability = new Ability();
        _pending_fighter->name = n;
        _pending_ability->name = n;
        return *this;
    }
};
static _NameSlot _name_slot;

struct _TypeSlot {
    _TypeSlot& operator=(const char* t) { _ensure_fighter()->type = t; return *this; }
};
static _TypeSlot _type_slot;

struct _HpSlot {
    _HpSlot& operator=(int v) { auto* f = _ensure_fighter(); f->hp = f->maxHp = v; return *this; }
};
static _HpSlot _hp_slot;

struct _ActionSlot {
    _ActionSlot& operator=(const std::function<void(Fighter*, Fighter*)>& fn) {
        _ensure_ability()->action = fn;
        return *this;
    }
};
static _ActionSlot _action_slot;

struct _FighterExpr {
    Fighter* f;
    _FighterExpr() : f(_pending_fighter ? _pending_fighter : new Fighter()) {
        _pending_fighter = nullptr;
        if (_pending_ability) { delete _pending_ability; _pending_ability = nullptr; }
        _temp_fighter = f;
    }
    _FighterExpr& operator,(const _Sink&) { return *this; }
    int operator=(std::initializer_list<_Sink>) {
        if (_game && f) _game->addFighter(f);
        _temp_fighter = nullptr;
        return 0;
    }
};

struct _AbilityExpr {
    Ability* a;
    _AbilityExpr() : a(_pending_ability ? _pending_ability : new Ability()) {
        _pending_ability = nullptr;
        if (_pending_fighter) { delete _pending_fighter; _pending_fighter = nullptr; }
        _temp_ability = a;
    }
    _AbilityExpr& operator,(const _Sink&) { return *this; }
    int operator=(std::initializer_list<_Sink>) {
        if (_game && a) _game->addAbility(a);
        _temp_ability = nullptr;
        return 0;
    }
};

struct _FighterBatch {
    template<typename T> _Sink operator[](T) {
        _temp_fighter = nullptr;
        return _sink;
    }
};
struct _AbilityBatch {
    template<typename T> _Sink operator[](T) {
        _temp_ability = nullptr;
        return _sink;
    }
};
static _FighterBatch _fighter_batch;
static _AbilityBatch _ability_batch;

struct _AttackerToken { Fighter* ptr; };
struct _DefenderToken { Fighter* ptr; };

struct _LoopCtx;
static _LoopCtx* _loop_ctx_current = nullptr;

struct _IfChain {
    std::function<bool()> any_true;
};
static std::vector<_IfChain> _if_stack;
static std::vector<std::function<bool()>> _pred_stack;

struct _LoopCtx {
    enum Kind { For, After };
    Kind kind;
    int rounds;
    Fighter* atk;
    Fighter* def;
    std::vector<std::function<void()>> cmds;
    _LoopCtx* prev;
    bool done;

    _LoopCtx(Kind k, int r, Fighter* a, Fighter* d)
        : kind(k), rounds(r), atk(a), def(d), prev(_loop_ctx_current), done(false) {
        _loop_ctx_current = this;
    }

    ~_LoopCtx() {
        _loop_ctx_current = prev;
        auto cmds_copy = cmds;
        auto act = [cmds_copy](Fighter*, Fighter*) {
            for (auto& fn : cmds_copy) fn();
        };
        //edw schedularoume ta commands mesa sto round pou erxetai
        if (_game) {
            if (kind == For) _game->scheduleFor(rounds, atk, def, act);
            else _game->scheduleAfter(rounds, atk, def, act);
        }
    }

    bool active() {
        if (done) return false;
        done = true;
        return true;
    }
};

inline bool _predicates_true(const std::vector<std::function<bool()>>& preds) {
    //ola ta if/else_if/else pernan apo edw gia na ginei to swsto filter
    for (const auto& p : preds) if (!p()) return false;
    return true;
}

inline void _run_or_queue(const std::function<void()>& fn) {
    //ola ta commands pernoun apo edo kai se for/after ginontai queue gia to sosto round
    if (_loop_ctx_current) {
        auto preds = _pred_stack;
        _loop_ctx_current->cmds.push_back([preds, fn]() {
            if (_predicates_true(preds)) fn();
        });
    } else {
        if (_predicates_true(_pred_stack)) fn();
    }
}

inline void _if_begin(const std::function<bool()>& cond) {
    //arxi if chain, krataei katastasi gia else_if/else
    _if_stack.push_back({cond});
    _pred_stack.push_back(cond);
}

inline void _if_close_branch() {
    if (!_pred_stack.empty()) _pred_stack.pop_back();
}

inline void _if_else_if(const std::function<bool()>& cond) {
    //else_if trexei mono an den trexthike kapoio proigoumeno branch
    if (_if_stack.empty()) return;
    auto prev_any = _if_stack.back().any_true;
    auto pred = [prev_any, cond]() { return !prev_any() && cond(); };
    _if_stack.back().any_true = [prev_any, cond]() { return prev_any() || cond(); };
    _pred_stack.push_back(pred);
}

inline void _if_else() {
    //else einai to teleutaio branch an den piasei kanena allo
    if (_if_stack.empty()) return;
    auto prev_any = _if_stack.back().any_true;
    auto pred = [prev_any]() { return !prev_any(); };
    _if_stack.back().any_true = []() { return true; };
    _pred_stack.push_back(pred);
}

inline void _if_end() {
    if (!_if_stack.empty()) _if_stack.pop_back();
}

struct _DamageCmd { Fighter* atk; };
struct _DamageTarget { Fighter* atk; Fighter* tgt; };
inline _DamageTarget operator,(_DamageCmd cmd, _DefenderToken tok) { return {cmd.atk, tok.ptr}; }
inline _DamageTarget operator,(_DamageCmd cmd, _AttackerToken tok) { return {cmd.atk, tok.ptr}; }
inline _Sink operator,(_DamageTarget dt, int amount) { _run_or_queue([=]() { if (_game) _game->doDamage(dt.atk, dt.tgt, amount); }); return _sink; }

struct _HealCmd { Fighter* atk; };
struct _HealTarget { Fighter* atk; Fighter* tgt; };
inline _HealTarget operator,(_HealCmd cmd, _DefenderToken tok) { return {cmd.atk, tok.ptr}; }
inline _HealTarget operator,(_HealCmd cmd, _AttackerToken tok) { return {cmd.atk, tok.ptr}; }
inline _Sink operator,(_HealTarget ht, int amount) { _run_or_queue([=]() { if (_game) _game->doHeal(ht.tgt, amount); }); return _sink; }

struct _TagCmd {};
struct _TagTarget { Fighter* tgt; };
inline _TagTarget operator,(_TagCmd, _DefenderToken tok) { return {tok.ptr}; }
inline _TagTarget operator,(_TagCmd, _AttackerToken tok) { return {tok.ptr}; }
struct _EnterRing {};
static const _EnterRing _ = {};
inline _Sink operator,(_TagTarget tt, _EnterRing) { _run_or_queue([=]() { tt.tgt->inRing = true; }); return _sink; }

struct _TagOutToken {};
static _TagOutToken a;
#define α a
inline _TagOutToken& operator-(_TagOutToken& t) { return t; }
inline _TagOutToken& operator--(_TagOutToken& t) { return t; }
inline _Sink operator,(_TagTarget tt, _TagOutToken&) { _run_or_queue([=]() { tt.tgt->inRing = false; }); return _sink; }

inline void _teach(const std::string& fighter, const char* ability) {
    if (_game) _game->teachAbility(fighter, ability);
}

#define BEGIN_GAME int main() { _game = new Game();
#define END_GAME ; delete _game; return 0; }

#define _CONCAT_IMPL(a, b) a##b
#define _CONCAT(a, b) _CONCAT_IMPL(a, b)

#define CREATE ;
#define FIGHTER _FighterExpr() =
#define FIGHTERS _fighter_batch
#define ABILITY _AbilityExpr() =
#define ABILITIES _ability_batch

#define NAME _sink, _name_slot = false ? (const char*)""
#define TYPE _sink, _type_slot = false ? (const char*)""
#define HP _sink, _hp_slot = false ? 0

#define ACTION _sink, _action_slot = [](Fighter* _ATTACKER_, Fighter* _DEFENDER_) { (void)(0 ? 0
#define START 0);
#define END ; }

#define DUEL ; _game->startDuel()

#define DAMAGE ; _DamageCmd{_ATTACKER_},
#define HEAL ; _HealCmd{_ATTACKER_},
#define TAG ; _TagCmd(),
#define ATTACKER _AttackerToken{_ATTACKER_},
#define DEFENDER _DefenderToken{_DEFENDER_},

inline int _get_hp(_DefenderToken tok) { return tok.ptr->hp; }
inline int _get_hp(_AttackerToken tok) { return tok.ptr->hp; }
inline int _get_hp(Fighter* f) { return f->hp; }
inline std::string _get_type(_DefenderToken tok) { return tok.ptr->type; }
inline std::string _get_type(_AttackerToken tok) { return tok.ptr->type; }
inline std::string _get_type(Fighter* f) { return f->type; }
inline std::string _get_name(_DefenderToken tok) { return tok.ptr->name; }
inline std::string _get_name(_AttackerToken tok) { return tok.ptr->name; }
inline std::string _get_name(Fighter* f) { return f->name; }
inline bool _is_out(_DefenderToken tok) { return !tok.ptr->inRing; }
inline bool _is_out(_AttackerToken tok) { return !tok.ptr->inRing; }
inline bool _is_out(Fighter* f) { return !f->inRing; }

#define GET_HP(x) _GET_HP_##x
#define GET_TYPE(x) _GET_TYPE_##x
#define GET_NAME(x) _GET_NAME_##x
#define IS_OUT_OF_RING(x) _GET_OUT_##x

#define _GET_HP_ATTACKER _get_hp(_AttackerToken{_ATTACKER_})
#define _GET_HP_DEFENDER _get_hp(_DefenderToken{_DEFENDER_})
#define _GET_TYPE_ATTACKER _get_type(_AttackerToken{_ATTACKER_})
#define _GET_TYPE_DEFENDER _get_type(_DefenderToken{_DEFENDER_})
#define _GET_NAME_ATTACKER _get_name(_AttackerToken{_ATTACKER_})
#define _GET_NAME_DEFENDER _get_name(_DefenderToken{_DEFENDER_})
#define _GET_OUT_ATTACKER _is_out(_AttackerToken{_ATTACKER_})
#define _GET_OUT_DEFENDER _is_out(_DefenderToken{_DEFENDER_})

template<typename T> inline bool _AND_impl(T v) { return static_cast<bool>(v); }
template<typename T, typename... Args> inline bool _AND_impl(T first, Args... rest) { return static_cast<bool>(first) && _AND_impl(rest...); }
template<typename T> inline bool _OR_impl(T v) { return static_cast<bool>(v); }
template<typename T, typename... Args> inline bool _OR_impl(T first, Args... rest) { return static_cast<bool>(first) || _OR_impl(rest...); }

#define AND(...) (_AND_impl(__VA_ARGS__))
#define OR(...) (_OR_impl(__VA_ARGS__))
#define NOT(x) (!(x))

#define IF _if_begin([&]() { return
#define DO_IF ; }); if (true) {
#define DO ) {
#define ELSE_IF ; } _if_close_branch(); _if_else_if([&]() { return
#define ELSE ; } _if_close_branch(); _if_else(); if (true) {
#define ENDIF ; } _if_close_branch(); _if_end();

#define FOR ; for (bool _loop_once = true; _loop_once; _loop_once = false) for (_LoopCtx _loop_ctx(_LoopCtx::Kind::For,
#define ROUNDS , _ATTACKER_, _DEFENDER_); _loop_ctx.active(); ) if (true
#define ENDFOR ; }

#define AFTER ; for (bool _loop_once = true; _loop_once; _loop_once = false) for (_LoopCtx _loop_ctx(_LoopCtx::Kind::After,
#define ENDAFTER ; }

struct _ShowCmd {
    std::ostringstream ss;
    template<typename T> _ShowCmd& operator<<(const T& v) { ss << v; return *this; }
    using _OManip = std::ostream& (*)(std::ostream&);
    _ShowCmd& operator<<(_OManip m) { m(ss); return *this; }
    ~_ShowCmd() {
        auto out = ss.str();
        _run_or_queue([out]() { std::cout << out; });
    }
};
#define SHOW ; _ShowCmd() <<

std::string _current_learn_fighter;
struct _LearnBatch {
    _Sink operator[](const char* block) {
        std::string s(block ? block : "");
        size_t pos = 0;
        while (pos < s.size()) {
            size_t end = s.find('\n', pos);
            if (end == std::string::npos) end = s.size();
            if (end > pos) {
                std::string line = s.substr(pos, end - pos);
                const std::string prefix = "@ABILITY:";
                if (line.compare(0, prefix.size(), prefix) == 0) {
                    std::string name = line.substr(prefix.size());
                    if (!name.empty()) _teach(_current_learn_fighter, name.c_str());
                }
            }
            pos = end + 1;
        }
        return _sink;
    }
};
static _LearnBatch _learn_batch;

#define DEAR ; _current_learn_fighter =
#define LEARN ; _learn_batch
#define ABILITY_NAME(x) "@ABILITY:" #x "\n"

#endif // TEKKEN_H
