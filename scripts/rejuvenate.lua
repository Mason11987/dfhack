-- This script will make any "old" dwarf 20 years old
-- usage is:  target a unit in DF, and execute this script in dfhack
-- the target will be changed to 20 years old
-- by vjek

function rejuvenate()
    local current_year,newbirthyear
    unit=dfhack.gui.getSelectedUnit()

    if unit==nil then print ("No unit under cursor!  Aborting.") return end

    current_year=df.global.cur_year
    newbirthyear=current_year - 20
    if unit.relations.birth_year < newbirthyear then
        unit.relations.birth_year=newbirthyear
    end
    if unit.relations.old_year < current_year+100 then
        unit.relations.old_year=current_year+100
    end
    print (dfhack.TranslateName(dfhack.units.getVisibleName(unit)).." is now 20 years old and will live at least 100 years")

end

rejuvenate()