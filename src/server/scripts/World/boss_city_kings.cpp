/*
 * Copyright (C) 2013-2016 JadeCore <https://www.jadecore.tk/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

enum KingSpells
{
    // King - Varian Wrynn
    SPELL_HEROIC_LEAP       = 59688,
    SPELL_WHIRLWIND         = 41056,

    // Queen - Tyrande Whisperwind
    SPELL_CLEAVE            = 20691,
    SPELL_MOONFIRE          = 20690,
    SPELL_SEARING_ARROW     = 20688,
    SPELL_STARFALL          = 20687,

    // King - Prophet Velen
    SPELL_HOLY_BLAST        = 59700,
    SPELL_HOLY_NOVA         = 59701,
    SPELL_HOLY_SMITE        = 59703,
    SPELL_PRAYER_OF_HEAL    = 59698,
    SPELL_STAFF_STRIKE      = 84647,

    // King - Muradin Bronzebeard + Adds
    SPELL_AVATAR            = 19135,
    SPELL_KNOCK_AWAY        = 20686,
    SPELL_STORM_BOLT        = 84831,

    // Falstad
    SPELL_CHAIN_LIGHTNING   = 84829,
    SPELL_SHOOT             = 84833,
    SPELL_STORM_HAMMER      = 84828,

    // King - Garrosh Hellscream
    SPELL_BERSERKER_CHARGE  = 84742,
    SPELL_DEMO_SHOUT        = 61044,
    SPELL_SHOCKWAVE         = 84715,

    // King - Kairne Bloodhoof
    SPELL_UPPERCUT          = 22916,
    SPELL_MORTAL_STRIKE     = 32736,
    SPELL_WAR_STOMP         = 59705,

    // Queen - Sylvanas Windrunner
    SPELL_BLACK_ARROW       = 59712,
    SPELL_FADE              = 20672,
    SPELL_FADE_BLINK        = 29211,
    SPELL_MULTI_SHOT        = 59713,
    SPELL_SHOT              = 59710,
    SPELL_SUMMON_SKELETON   = 59711,
    SPELL_HIGHBORNE_AURA    = 37090,
    SPELL_SYLVANAS_CAST     = 36568,
    SPELL_RIBBON_OF_SOULS   = 34432, // The real one to use might be 37099
};

enum Misc
{
    QUEST_JOURNEY_TO_UNDERCITY  = 9180,
    EMOTE_LAMENT_END            = 0,
    SAY_LAMENT_END              = 1,
    SOUND_CREDIT                = 10896,
    NPC_HIGHBORNE_LAMENTER      = 21628,
    NPC_HIGHBORNE_BUNNY         = 21641,
    NPC_SYL_SKELETON            = 45285
};

float HighborneLoc[4][3]=
{
    {1285.41f, 312.47f, 0.51f},
    {1286.96f, 310.40f, 1.00f},
    {1289.66f, 309.66f, 1.52f},
    {1292.51f, 310.50f, 1.99f},
};

#define HIGHBORNE_LOC_Y       -61.00f
#define HIGHBORNE_LOC_Y_NEW   -55.50f

enum KingEvents
{
    // King - Varian Wrynn
    EVENT_HEROIC_LEAP       = 1,
    EVENT_WHIRLWIND,
    EVENT_GUARDS,

    // Queen - Tyrande Whisperwind
    EVENT_CLEAVE,
    EVENT_MOONFIRE,
    EVENT_SEARING_ARROW,
    EVENT_STARFALL,

    // King - Prophet Velen
    EVENT_HOLY_BLAST,
    EVENT_HOLY_NOVA,
    EVENT_HOLY_SMITE,
    EVENT_PRAYER_OF_HEAL,
    EVENT_STAFF_STRIKE,

    // King - Muradin Bronzebeard
    EVENT_AVATAR,
    EVENT_KNOCK_AWAY,
    EVENT_STORM_BOLT,

    EVENT_CHAIN_LIGHTNING,
    EVENT_SHOOT,
    EVENT_STORM_HAMMER,

    // King - Garrosh Hellscream
    EVENT_BERSEKER_CHARGE,
    EVENT_DEMO_SHOUT,
    EVENT_SHOCKWAVE,
    EVENT_GAR_WHIRLWIND,
    EVENT_WAR_STOMP,

    // King - Kairne Bloodhoof
    EVENT_MORTAL_STRIKE,
    EVENT_UPPERCUT,

    // Queen - Sylvanas Windrunner
    EVENT_FADE,
    EVENT_MULTI_SHOT,
    EVENT_SUMMON_SKELETON,
    EVENT_SYL_SHOT,
    EVENT_BLACK_ARROW
};

enum KingTexts
{
    SAY_VARIAN_WRYNN_AGGRO         = 0,
    SAY_VARIAN_WRYNN_GUARDS        = 1,
    SAY_VARIAN_WRYNN_DEATH         = 2,

    SAY_TYRANDE_WHISPERWIND_AGGRO  = 0,
    SAY_TYRANDE_WHISPERWIND_DEATH  = 1
};

Position const PosWrynnGuards[2] =
{
    { -8363.64f, 254.42f, 155.34f, 3.76f }, // Wrynn Guards
    { -8384.68f, 237.71f, 155.34f, 0.63f },
};

enum KingSummons
{
    NPC_VARIAN_WRYNN_GUARDS        = 1756
};

/*######
##  boss_varian_wrynn
######*/

class boss_varian_wrynn : public CreatureScript
{
public:
    boss_varian_wrynn() : CreatureScript("boss_varian_wrynn") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_varian_wrynnAI (creature);
    }

    struct boss_varian_wrynnAI : public ScriptedAI
    {
        boss_varian_wrynnAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

        SummonList summons;

        void Reset()
        {
            summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_VARIAN_WRYNN_AGGRO);
            events.ScheduleEvent(EVENT_HEROIC_LEAP, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_WHIRLWIND, 25*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_GUARDS, 30*IN_MILLISECONDS);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() != NPC_VARIAN_WRYNN_GUARDS)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                    summon->AI()->AttackStart(target);
                    summon->SetInCombatWithZone();

                summons.Summon(summon);
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_VARIAN_WRYNN_DEATH);
            summons.DespawnAll();
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_HEROIC_LEAP:
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_HEROIC_LEAP);
                    events.ScheduleEvent(EVENT_HEROIC_LEAP, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                    break;
                case EVENT_WHIRLWIND:
                    DoCastVictim(SPELL_WHIRLWIND);
                    events.ScheduleEvent(EVENT_WHIRLWIND, urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                case EVENT_GUARDS:
                    Talk(SAY_VARIAN_WRYNN_GUARDS);
                    for (uint8 i = 0; i < 2; ++i)
                        me->SummonCreature(NPC_VARIAN_WRYNN_GUARDS, PosWrynnGuards[i], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5000);
                    events.ScheduleEvent(EVENT_GUARDS, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_tyrande_whisperwind
######*/

class boss_tyrande_whisperwind : public CreatureScript
{
public:
    boss_tyrande_whisperwind() : CreatureScript("boss_tyrande_whisperwind") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_tyrande_whisperwindAI (creature);
    }

    struct boss_tyrande_whisperwindAI : public ScriptedAI
    {
        boss_tyrande_whisperwindAI(Creature* creature) : ScriptedAI(creature) { }

        Player* Random;

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_TYRANDE_WHISPERWIND_AGGRO);
            events.ScheduleEvent(EVENT_CLEAVE, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_MOONFIRE, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SEARING_ARROW, 15*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_STARFALL, 30*IN_MILLISECONDS);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_TYRANDE_WHISPERWIND_DEATH);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    events.ScheduleEvent(EVENT_CLEAVE, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    break;
                case EVENT_MOONFIRE:
                    Random = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true)->ToPlayer();
                    DoCast(Random, SPELL_MOONFIRE);
                    events.ScheduleEvent(EVENT_MOONFIRE, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                    break;
                case EVENT_SEARING_ARROW:
                    Random = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true)->ToPlayer();
                    DoCast(Random, SPELL_SEARING_ARROW);
                    events.ScheduleEvent(EVENT_SEARING_ARROW, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                    break;
                case EVENT_STARFALL:
                    Random = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true)->ToPlayer();
                    DoCast(Random, SPELL_STARFALL);
                    events.ScheduleEvent(EVENT_STARFALL, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_prophet_velen
######*/

class boss_prophet_velen : public CreatureScript
{
public:
    boss_prophet_velen() : CreatureScript("boss_prophet_velen") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_prophet_velenAI (creature);
    }

    struct boss_prophet_velenAI : public ScriptedAI
    {
        boss_prophet_velenAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_HOLY_BLAST, 9*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_HOLY_NOVA, 30*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_HOLY_SMITE, 31*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_PRAYER_OF_HEAL, 31*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_STAFF_STRIKE, 31*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_HOLY_BLAST:
                    DoCastVictim(SPELL_HOLY_BLAST);
                    events.ScheduleEvent(EVENT_HOLY_BLAST, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    break;
                case EVENT_HOLY_NOVA:
                    DoCastAOE(SPELL_HOLY_NOVA);
                    events.ScheduleEvent(EVENT_HOLY_NOVA, urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                case EVENT_HOLY_SMITE:
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_HOLY_SMITE);
                    events.ScheduleEvent(EVENT_HOLY_SMITE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    break;
                case EVENT_PRAYER_OF_HEAL:
                    DoCast(SPELL_PRAYER_OF_HEAL);
                    events.ScheduleEvent(EVENT_PRAYER_OF_HEAL, urand(45*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                    break;
                case EVENT_STAFF_STRIKE:
                    DoCastVictim(SPELL_STAFF_STRIKE);
                    events.ScheduleEvent(EVENT_STAFF_STRIKE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_muradin_bronzebeard
######*/

class boss_muradin_bronzebeard : public CreatureScript
{
public:
    boss_muradin_bronzebeard() : CreatureScript("boss_muradin_bronzebeard") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_muradin_bronzebeardAI (creature);
    }

    struct boss_muradin_bronzebeardAI : public ScriptedAI
    {
        boss_muradin_bronzebeardAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_AVATAR, 40*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_KNOCK_AWAY, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_STORM_BOLT, 25*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_AVATAR:
                    DoCast(SPELL_AVATAR);
                    events.ScheduleEvent(EVENT_AVATAR, urand(50*IN_MILLISECONDS, 70*IN_MILLISECONDS));
                    break;
                case EVENT_KNOCK_AWAY:
                    DoCastVictim(SPELL_KNOCK_AWAY);
                    events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(30*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                case EVENT_STORM_BOLT:
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_STORM_BOLT);
                    events.ScheduleEvent(EVENT_STORM_BOLT, urand(40*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_falstad_wildhammer
######*/

class boss_falstad_wildhammer : public CreatureScript
{
public:
    boss_falstad_wildhammer() : CreatureScript("boss_falstad_wildhammer") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_falstad_wildhammerAI (creature);
    }

    struct boss_falstad_wildhammerAI : public ScriptedAI
    {
        boss_falstad_wildhammerAI(Creature* creature) : ScriptedAI(creature) { }

        Player* Random;

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SHOOT, 25*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_STORM_HAMMER, 15*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_CHAIN_LIGHTNING:
                    Random = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true)->ToPlayer();
                    DoCast(Random, SPELL_CHAIN_LIGHTNING);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(45*IN_MILLISECONDS, 50*IN_MILLISECONDS));
                    break;
                case EVENT_SHOOT:
                    DoCast(SelectTarget(SELECT_TARGET_FARTHEST, 0, 30.0f, true), SPELL_SHOOT);
                    events.ScheduleEvent(EVENT_SHOOT, urand(25*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    break;
                case EVENT_STORM_HAMMER:
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_STORM_HAMMER);
                    events.ScheduleEvent(EVENT_STORM_HAMMER, urand(30*IN_MILLISECONDS, 50*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_garrosh_hellscream
######*/

class boss_garrosh_hellscream : public CreatureScript
{
public:
    boss_garrosh_hellscream() : CreatureScript("boss_garrosh_hellscream") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_garrosh_hellscreamAI (creature);
    }

    struct boss_garrosh_hellscreamAI : public ScriptedAI
    {
        boss_garrosh_hellscreamAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_BERSEKER_CHARGE, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_DEMO_SHOUT, 5*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SHOCKWAVE, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_WHIRLWIND, 40*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_BERSEKER_CHARGE:
                    DoCast(SelectTarget(SELECT_TARGET_FARTHEST, 0, 40.0f, true), SPELL_BERSERKER_CHARGE);
                    events.ScheduleEvent(EVENT_BERSEKER_CHARGE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    break;
                case EVENT_DEMO_SHOUT:
                    DoCast(SPELL_DEMO_SHOUT);
                    events.ScheduleEvent(EVENT_DEMO_SHOUT, urand(50*IN_MILLISECONDS, 80*IN_MILLISECONDS));
                    break;
                case EVENT_SHOCKWAVE:
                    DoCastVictim(SPELL_SHOCKWAVE);
                    events.ScheduleEvent(EVENT_SHOCKWAVE, urand(40*IN_MILLISECONDS, 50*IN_MILLISECONDS));
                    break;
                case EVENT_GAR_WHIRLWIND:
                    DoCastVictim(SPELL_WHIRLWIND);
                    events.ScheduleEvent(EVENT_GAR_WHIRLWIND, urand(1*MINUTE*IN_MILLISECONDS, 2*MINUTE*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_baine_bloodhoof
######*/

class boss_baine_bloodhoof : public CreatureScript
{
public:
    boss_baine_bloodhoof() : CreatureScript("boss_baine_bloodhoof") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_baine_bloodhoofAI (creature);
    }

    struct boss_baine_bloodhoofAI : public ScriptedAI
    {
        boss_baine_bloodhoofAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_MORTAL_STRIKE, 9*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_UPPERCUT, 30*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_WAR_STOMP, 31*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_MORTAL_STRIKE:
                    DoCastVictim(SPELL_MORTAL_STRIKE);
                    events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                    break;
                case EVENT_UPPERCUT:
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_UPPERCUT);
                    events.ScheduleEvent(EVENT_UPPERCUT, urand(30*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                case EVENT_WAR_STOMP:
                    DoCast(SPELL_WAR_STOMP);
                    events.ScheduleEvent(EVENT_WAR_STOMP, urand(25*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
##  boss_lady_sylvanas_windrunner
######*/

class boss_lady_sylvanas_windrunner : public CreatureScript
{
public:
    boss_lady_sylvanas_windrunner() : CreatureScript("boss_lady_sylvanas_windrunner") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_lady_sylvanas_windrunnerAI (creature);
    }

    bool OnQuestReward(Player* /*player*/, Creature* creature, const Quest *_Quest, uint32 /*slot*/) override
    {
        if (_Quest->GetQuestId() == QUEST_JOURNEY_TO_UNDERCITY)
        {
            CAST_AI(boss_lady_sylvanas_windrunner::boss_lady_sylvanas_windrunnerAI, creature->AI())->LamentEvent = true;
            CAST_AI(boss_lady_sylvanas_windrunner::boss_lady_sylvanas_windrunnerAI, creature->AI())->DoPlaySoundToSet(creature, SOUND_CREDIT);
            creature->CastSpell(creature, SPELL_SYLVANAS_CAST, false);

            for (uint8 i = 0; i < 4; ++i)
                creature->SummonCreature(NPC_HIGHBORNE_LAMENTER, HighborneLoc[i][0], HighborneLoc[i][1], HIGHBORNE_LOC_Y, HighborneLoc[i][2], TEMPSUMMON_TIMED_DESPAWN, 160000);
        }

        return true;
    }

    struct boss_lady_sylvanas_windrunnerAI : public ScriptedAI
    {
        boss_lady_sylvanas_windrunnerAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 LamentEventTimer;
        bool LamentEvent;
        uint64 targetGUID;

        void Reset() override
        {
            LamentEventTimer = 5000;
            LamentEvent = false;
            targetGUID = 0;
            DespawnMinions(NPC_SYL_SKELETON);
        }

        void DespawnMinions(uint32 entry)
        {
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);
            if (creatures.empty())
                return;
            for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                (*iter)->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_FADE, 30*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SUMMON_SKELETON, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BLACK_ARROW, 15*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SYL_SHOT, 8*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_MULTI_SHOT, 10*IN_MILLISECONDS);
        }

        void JustSummoned(Creature* summoned) override
        {
            if (summoned->GetEntry() == NPC_HIGHBORNE_BUNNY)
            {
                if (Creature* target = Unit::GetCreature(*summoned, targetGUID))
                {
                    target->MonsterMoveWithSpeed(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0);
                    target->SetPosition(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0.0f);
                    summoned->CastSpell(target, SPELL_RIBBON_OF_SOULS, false);
                }

                summoned->SetDisableGravity(true);
                targetGUID = summoned->GetGUID();
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
           DespawnMinions(NPC_SYL_SKELETON);
        }

        void UpdateAI(uint32 diff) override
        {
            if (LamentEvent)
            {
                if (LamentEventTimer <= diff)
                {
                    DoSummon(NPC_HIGHBORNE_BUNNY, me, 10.0f, 3000, TEMPSUMMON_TIMED_DESPAWN);

                    LamentEventTimer = 2000;
                    if (!me->HasAura(SPELL_SYLVANAS_CAST))
                    {
                        Talk(SAY_LAMENT_END);
                        Talk(EMOTE_LAMENT_END);
                        LamentEvent = false;
                    }
                } else LamentEventTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_FADE:
                    DoCast(me, SPELL_FADE);
                    DoCast(me, SPELL_FADE_BLINK);
                    if (Unit* victim = me->GetVictim())
                        if (me->GetDistance(victim) > 10.0f)
                            DoCastVictim(SPELL_MULTI_SHOT);
                    events.ScheduleEvent(EVENT_FADE, 30000 + rand()%5000);
                    break;
                case EVENT_SUMMON_SKELETON:
                    DoCast(SPELL_SUMMON_SKELETON);
                    events.ScheduleEvent(EVENT_SUMMON_SKELETON, 20000 + rand()%10000);
                    break;
                case EVENT_BLACK_ARROW:
                    DoCastVictim(SPELL_BLACK_ARROW);
                    events.ScheduleEvent(EVENT_BLACK_ARROW, 15000 + rand()%5000);
                    break;
                case EVENT_SYL_SHOT:
                    DoCastVictim(SPELL_SHOT);
                    events.ScheduleEvent(EVENT_SYL_SHOT, 8000 + rand()%2000);
                    break;
                case EVENT_MULTI_SHOT:
                    DoCastVictim(SPELL_MULTI_SHOT);
                    events.ScheduleEvent(EVENT_MULTI_SHOT, 10000 + rand()%3000);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

/*######
## npc_highborne_lamenter
######*/

class npc_highborne_lamenter : public CreatureScript
{
public:
    npc_highborne_lamenter() : CreatureScript("npc_highborne_lamenter") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highborne_lamenterAI(creature);
    }

    struct npc_highborne_lamenterAI : public ScriptedAI
    {
        npc_highborne_lamenterAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 EventMoveTimer;
        uint32 EventCastTimer;
        bool EventMove;
        bool EventCast;

        void Reset() override
        {
            EventMoveTimer = 10*IN_MILLISECONDS;
            EventCastTimer = 17.5*IN_MILLISECONDS;
            EventMove = true;
            EventCast = true;
        }

        void EnterCombat(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            if (EventMove)
            {
                if (EventMoveTimer <= diff)
                {
                    me->SetDisableGravity(true);
                    me->MonsterMoveWithSpeed(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetDistance(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW) / (5000 * 0.001f));
                    me->SetPosition(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetOrientation());
                    EventMove = false;
                } else EventMoveTimer -= diff;
            }
            if (EventCast)
            {
                if (EventCastTimer <= diff)
                {
                    DoCast(me, SPELL_HIGHBORNE_AURA);
                    EventCast = false;
                } else EventCastTimer -= diff;
            }
        }
    };
};

void AddSC_boss_city_kings()
{
    new boss_varian_wrynn();
    new boss_tyrande_whisperwind();
    new boss_prophet_velen();
    new boss_muradin_bronzebeard();
    new boss_falstad_wildhammer();
    new boss_garrosh_hellscream();
    new boss_baine_bloodhoof();
    new boss_lady_sylvanas_windrunner();
    new npc_highborne_lamenter();
};
