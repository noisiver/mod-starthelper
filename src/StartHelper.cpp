#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

enum Proficiencies
{
    ONE_HANDED_MACES  = 198,
    POLEARMS          = 200,
    ONE_HANDED_SWORDS = 201,
    TWO_HANDED_SWORDS = 202,
    STAVES            = 227,
    BOWS              = 264,
    DUAL_WIELD        = 674,
    PLATE_MAIL        = 750,
    DAGGERS           = 1180,
    WANDS             = 5009,
    MAIL              = 8737
};

class StartHelperCommand : public CommandScript
{
    public:
        StartHelperCommand() : CommandScript("StartHelperCommand") {}

        ChatCommandTable GetCommands() const override
        {
            static ChatCommandTable commandTable =
            {
                { "starthelper", HandleSpecializationCommand, SEC_PLAYER, Console::No }
            };

            return commandTable;
        }

        static bool HandleSpecializationCommand(ChatHandler* handler, Optional<std::string> spec)
        {
            Player* player = handler->GetPlayer();
            uint8 playerClass = player->getClass();
            uint8 playerLevel = player->getLevel();

            if (playerClass == CLASS_DEATH_KNIGHT)
            {
                ChatHandler(handler->GetSession()).SendSysMessage("Your class is not |cffFF0000supported|r.");
                return true;
            }

            if (!spec)
            {
                SendInformationToPlayer(handler, playerClass);
                return true;
            }

            if (playerLevel > 60)
            {
                ChatHandler(handler->GetSession()).SendSysMessage("This feature can |cffFF0000only|r be used at or below level 60.");
                return true;
            }

            if ((playerClass == CLASS_WARRIOR && stricmp(spec.value().c_str(), "arms") != 0 && stricmp(spec.value().c_str(), "fury") != 0 && stricmp(spec.value().c_str(), "protection") != 0) ||
                (playerClass == CLASS_PALADIN && stricmp(spec.value().c_str(), "holy") != 0 && stricmp(spec.value().c_str(), "protection") != 0 && stricmp(spec.value().c_str(), "retribution") != 0) ||
                (playerClass == CLASS_HUNTER && stricmp(spec.value().c_str(), "beastmastery") != 0 && stricmp(spec.value().c_str(), "marksmanship") != 0 && stricmp(spec.value().c_str(), "survival") != 0) ||
                (playerClass == CLASS_ROGUE && stricmp(spec.value().c_str(), "assassination") != 0 && stricmp(spec.value().c_str(), "combat") != 0 && stricmp(spec.value().c_str(), "subtlety") != 0) ||
                (playerClass == CLASS_PRIEST && stricmp(spec.value().c_str(), "discipline") != 0 && stricmp(spec.value().c_str(), "holy") != 0 && stricmp(spec.value().c_str(), "shadow") != 0) ||
                (playerClass == CLASS_SHAMAN && stricmp(spec.value().c_str(), "elemental") != 0 && stricmp(spec.value().c_str(), "enhancement") != 0 && stricmp(spec.value().c_str(), "restoration") != 0) ||
                (playerClass == CLASS_MAGE && stricmp(spec.value().c_str(), "arcane") != 0 && stricmp(spec.value().c_str(), "fire") != 0 && stricmp(spec.value().c_str(), "frost") != 0) ||
                (playerClass == CLASS_WARLOCK && stricmp(spec.value().c_str(), "affliction") != 0 && stricmp(spec.value().c_str(), "demonology") != 0 && stricmp(spec.value().c_str(), "destruction") != 0) ||
                (playerClass == CLASS_DRUID && stricmp(spec.value().c_str(), "balance") != 0 && stricmp(spec.value().c_str(), "feral") != 0 && stricmp(spec.value().c_str(), "restoration") != 0))
            {
                ChatHandler(handler->GetSession()).SendSysMessage("The |cffFF0000specialization|r is not valid.");
                return true;
            }

            if (playerLevel < 60)
                player->SetLevel(60, true);

            player->resetTalents(true);
            player->InitTalentForLevel();
            player->UpdateSkillsForLevel();

            if (playerClass == CLASS_HUNTER)
                player->SendTalentsInfoData(true);

            AddEquipmentToPlayer(player, playerClass, spec);
            player->UpdateSkillsToMaxSkillsForLevel();
            player->SetFullHealth();
            player->ResetAllPowers();
            TeleportPlayerToCity(player);

            return true;
        }

    private:
        static void SendInformationToPlayer(ChatHandler* handler, uint8 playerClass)
        {
            ChatHandler(handler->GetSession()).SendSysMessage("This feature will set your level to |cff4CFF0060|r, give you a |cff4CFF00full set of equipment|r, |cffFF0000destroy|r every piece of equipment you currently have equipped and teleport you to a major city.");
            ChatHandler(handler->GetSession()).SendSysMessage("The accepted commands for your class are:");

            switch (playerClass)
            {
                case CLASS_WARRIOR:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper arms/fury/protection|r");
                    break;
                case CLASS_PALADIN:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper holy/protection/retribution|r");
                    break;
                case CLASS_HUNTER:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper beastmastery/marksmanship/survival|r");
                    break;
                case CLASS_ROGUE:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper assassination/combat/subtlety|r");
                    break;
                case CLASS_PRIEST:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper discipline/holy/shadow|r");
                    break;
                case CLASS_SHAMAN:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper elemental/enhancement/restoration|r");
                    break;
                case CLASS_MAGE:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper arcane/fire/frost|r");
                    break;
                case CLASS_WARLOCK:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper affliction/demonology/destruction|r");
                    break;
                case CLASS_DRUID:
                    ChatHandler(handler->GetSession()).SendSysMessage("|cff4CFF00.starthelper balance/feral/restoration|r");
                    break;
                default:
                    break;
            }
        }

        static void AddEquipmentToPlayer(Player* player, uint8 playerClass, Optional<std::string> spec)
        {
            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++) player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

            if (playerClass == CLASS_WARRIOR)
            {
                if (!player->HasSpell(Proficiencies::ONE_HANDED_SWORDS))
                    player->learnSpell(Proficiencies::ONE_HANDED_SWORDS);

                if (!player->HasSpell(Proficiencies::BOWS))
                    player->learnSpell(Proficiencies::BOWS);

                if (!player->HasSpell(Proficiencies::PLATE_MAIL))
                    player->learnSpell(Proficiencies::PLATE_MAIL);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 20263, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 13166, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 22712, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 11678, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 12936, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 22714, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 13142, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 21495, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 21490, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 20721, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 22321, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 11815, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 24020, true);

                if (stricmp(spec.value().c_str(), "arms") == 0 || stricmp(spec.value().c_str(), "fury") == 0)
                {
                    if (!player->HasSpell(Proficiencies::DUAL_WIELD))
                        player->learnSpell(Proficiencies::DUAL_WIELD);

                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19901, true);
                }
                else
                {
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 12602, true);
                }

                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 22318, true);
            }
            else if (playerClass == CLASS_PALADIN)
            {
                if (!player->HasSpell(Proficiencies::PLATE_MAIL))
                    player->learnSpell(Proficiencies::PLATE_MAIL);

                if (stricmp(spec.value().c_str(), "holy") == 0)
                {
                    if (!player->HasSpell(Proficiencies::ONE_HANDED_MACES))
                        player->learnSpell(Proficiencies::ONE_HANDED_MACES);

                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 21803, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 12625, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13168, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 18741, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 12631, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 18702, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 20266, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 20265, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24045, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 24154, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 22713, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19915, true);
                }
                else
                {
                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 20263, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 13166, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 22712, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 11678, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 12936, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 22714, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 13142, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 21495, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 21490, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 20721, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 22321, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 11815, true);

                    if (stricmp(spec.value().c_str(), "protection") == 0)
                    {
                        if (!player->HasSpell(Proficiencies::ONE_HANDED_SWORDS))
                            player->learnSpell(Proficiencies::ONE_HANDED_SWORDS);

                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 24020, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 12602, true);
                    }
                    else
                    {
                        if (!player->HasSpell(Proficiencies::TWO_HANDED_SWORDS))
                            player->learnSpell(Proficiencies::TWO_HANDED_SWORDS);

                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 31134, true);
                    }
                }
            }
            else if (playerClass == CLASS_HUNTER)
            {
                if (!player->HasSpell(Proficiencies::POLEARMS))
                    player->learnSpell(Proficiencies::POLEARMS);

                if (!player->HasSpell(Proficiencies::BOWS))
                    player->learnSpell(Proficiencies::BOWS);

                if (!player->HasSpell(Proficiencies::MAIL))
                    player->learnSpell(Proficiencies::MAIL);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 13359, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 18686, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 19907, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 18530, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 21502, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 22715, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 18393, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 19887, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 18047, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 20721, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 13213, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 18537, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 22314, true);
                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 19993, true);

                player->AddItem(28053, 1000);
            }
            else if (playerClass == CLASS_ROGUE)
            {
                if (!player->HasSpell(Proficiencies::BOWS))
                    player->learnSpell(Proficiencies::BOWS);

                if (!player->HasSpell(Proficiencies::DUAL_WIELD))
                    player->learnSpell(Proficiencies::DUAL_WIELD);

                if (!player->HasSpell(Proficiencies::DAGGERS))
                    player->learnSpell(Proficiencies::DAGGERS);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 22718, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 13358, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 19907, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 12603, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 24023, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 19869, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 31131, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 19889, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 19906, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 13098, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 22321, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 11815, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 5267, true);
                player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19542, true);
                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 19993, true);
            }
            else if (playerClass == CLASS_PRIEST)
            {
                if (!player->HasSpell(Proficiencies::DAGGERS))
                    player->learnSpell(Proficiencies::DAGGERS);

                if (!player->HasSpell(Proficiencies::WANDS))
                    player->learnSpell(Proficiencies::WANDS);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 19886, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 24024, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13346, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 21496, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 18407, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 21500, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 31133, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 21810, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24154, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 21483, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 19965, true);
                player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19922, true);
                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 22408, true);
            }
            else if (playerClass == CLASS_SHAMAN)
            {
                if (!player->HasSpell(Proficiencies::ONE_HANDED_MACES))
                    player->learnSpell(Proficiencies::ONE_HANDED_MACES);

                if (!player->HasSpell(Proficiencies::MAIL))
                    player->learnSpell(Proficiencies::MAIL);

                if (stricmp(spec.value().c_str(), "enhancement") == 0)
                {
                    if (!player->HasSpell(Proficiencies::DUAL_WIELD))
                        player->learnSpell(Proficiencies::DUAL_WIELD);

                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 13359, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 18686, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 19907, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 18530, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 21502, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 22715, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 18393, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 19887, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 18047, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 20721, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 22321, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 11815, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 19961, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 22322, true);
                }
                else
                {
                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 21804, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 14548, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13123, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 13969, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 13344, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 18104, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 14522, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 20262, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24154, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 21483, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 22713, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19915, true);
                }
            }
            else if (playerClass == CLASS_MAGE)
            {
                if (!player->HasSpell(Proficiencies::DAGGERS))
                    player->learnSpell(Proficiencies::DAGGERS);

                if (!player->HasSpell(Proficiencies::WANDS))
                    player->learnSpell(Proficiencies::WANDS);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 19886, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 24024, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13346, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 21496, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 18407, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 21500, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 31133, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 21810, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24154, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 21483, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 19965, true);
                player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19922, true);
                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 22408, true);
            }
            else if (playerClass == CLASS_WARLOCK)
            {
                if (!player->HasSpell(Proficiencies::DAGGERS))
                    player->learnSpell(Proficiencies::DAGGERS);

                if (!player->HasSpell(Proficiencies::WANDS))
                    player->learnSpell(Proficiencies::WANDS);

                player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 19886, true);
                player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 24024, true);
                player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13346, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 21496, true);
                player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 18407, true);
                player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 21500, true);
                player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 31133, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FEET, 21810, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24154, true);
                player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 21483, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 19965, true);
                player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19922, true);
                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 22408, true);
            }
            else if (playerClass == CLASS_DRUID)
            {
                if (stricmp(spec.value().c_str(), "balance") == 0 || stricmp(spec.value().c_str(), "restoration") == 0)
                {
                    if (!player->HasSpell(Proficiencies::STAVES))
                        player->learnSpell(Proficiencies::STAVES);

                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 21484, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24096, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 19928, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 21470, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 13092, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 18525, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 21469, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 21494, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 19877, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 19892, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 24154, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 21483, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 18371, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 12930, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 24069, true);
                }
                else if (stricmp(spec.value().c_str(), "feral") == 0)
                {
                    if (!player->HasSpell(Proficiencies::POLEARMS))
                        player->learnSpell(Proficiencies::POLEARMS);

                    player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 22718, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 24073, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 13358, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 19907, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 12603, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 24023, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 19869, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 31131, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 19889, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FEET, 19906, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 21477, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 13098, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 22321, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 11815, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 22314, true);
                }
            }
        }

        static void TeleportPlayerToCity(Player* player)
        {
            uint32 mapId;
            //uint32 areaId;
            float x;
            float y;
            float z;
            float orientation;

            if (player->GetTeamId() == TEAM_ALLIANCE)
            {
                mapId       = 0;
                //areaId      = 1519;
                x           = -8830.44;
                y           = 626.666;
                z           = 93.9829;
                orientation = 0.682076;
            }
            else
            {
                mapId       = 1;
                //areaId      = 1637;
                x           = 1630.78;
                y           = -4412.99;
                z           = 16.5677;
                orientation = 0.080535;
            }

            //const WorldLocation& location = WorldLocation(mapId, x, y, z, orientation);

            player->TeleportTo(mapId, x, y, z, orientation);
            //player->SetHomebind(location, areaId);
        }
};

void AddStartHelperScripts()
{
    new StartHelperCommand();
}
