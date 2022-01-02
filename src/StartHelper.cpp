#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

enum Proficiencies
{
    ONE_HANDED_MACES  = 198,
    ONE_HANDED_SWORDS = 201,
    TWO_HANDED_SWORDS = 202,
    BOWS              = 264,
    DUAL_WIELD        = 674,
    PLATE_MAIL        = 750,
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
                ChatHandler(handler->GetSession()).SendSysMessage("This feature will set your level to |cff4CFF0060|r, give you a |cff4CFF00full set of equipment|r and |cffFF0000destroy|r every piece of equipment you currently have equipped.");
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

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
                player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

            if (playerClass == CLASS_WARRIOR)
            {
                if (!player->HasSpell(Proficiencies::ONE_HANDED_SWORDS))
                    player->learnSpell(Proficiencies::ONE_HANDED_SWORDS);

                if (!player->HasSpell(Proficiencies::BOWS))
                    player->learnSpell(Proficiencies::BOWS);


                if (stricmp(spec.value().c_str(), "arms") == 0 || stricmp(spec.value().c_str(), "fury") == 0)
                    if (!player->HasSpell(Proficiencies::DUAL_WIELD))
                        player->learnSpell(Proficiencies::DUAL_WIELD);

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
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 19901, true);
                }
                else if (stricmp(spec.value().c_str(), "protection") == 0)
                {
                    player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 12602, true);
                }

                player->EquipNewItem(EQUIPMENT_SLOT_RANGED, 22318, true);
            }
            else if (playerClass == CLASS_PALADIN)
            {
                if (stricmp(spec.value().c_str(), "holy") == 0)
                    if (!player->HasSpell(Proficiencies::ONE_HANDED_MACES))
                        player->learnSpell(Proficiencies::ONE_HANDED_MACES);

                if (stricmp(spec.value().c_str(), "protection") == 0)
                    if (!player->HasSpell(Proficiencies::ONE_HANDED_SWORDS))
                        player->learnSpell(Proficiencies::ONE_HANDED_SWORDS);

                if (stricmp(spec.value().c_str(), "retribution") == 0)
                    if (!player->HasSpell(Proficiencies::TWO_HANDED_SWORDS))
                        player->learnSpell(Proficiencies::TWO_HANDED_SWORDS);

                if (!player->HasSpell(Proficiencies::PLATE_MAIL))
                    player->learnSpell(Proficiencies::PLATE_MAIL);

                if (stricmp(spec.value().c_str(), "holy") == 0)
                {
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
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 24020, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 12602, true);
                    }
                    else
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 31134, true);
                    }
                }
            }
            else if (playerClass == CLASS_HUNTER)
            {
                if (!player->HasSpell(Proficiencies::MAIL))
                    player->learnSpell(Proficiencies::MAIL);
            }
            else if (playerClass == CLASS_ROGUE)
            {
            }
            else if (playerClass == CLASS_PRIEST)
            {
            }
            else if (playerClass == CLASS_SHAMAN)
            {
                if (!player->HasSpell(Proficiencies::MAIL))
                    player->learnSpell(Proficiencies::MAIL);
            }
            else if (playerClass == CLASS_MAGE)
            {
            }
            else if (playerClass == CLASS_WARLOCK)
            {
            }
            else if (playerClass == CLASS_DRUID)
            {
            }

            player->UpdateSkillsToMaxSkillsForLevel();
            player->SetFullHealth();
            player->ResetAllPowers();
            return true;
        }

    private:
};

void AddStartHelperScripts()
{
    new StartHelperCommand();
}
