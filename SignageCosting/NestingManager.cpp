#include "NestingManager.h"
#include "MaterialDatabase.h"
#include "NestingCore.h"

std::vector<NestGroup> NestingManager::buildGroups(const Job& job)
{
    std::vector<NestGroup> groups;

    for (size_t i = 0; i < job.items.size(); i++)
    {
        const auto& item = job.items[i];

        const Material& m = item.material;

        const MaterialVariant& v =
            m.variants[item.variantIndex];

        // ------------------------------------
        // BUILD NEST GROUPS
        // ------------------------------------

        NestGroup* group = nullptr;

        for (auto& g : groups)
        {
            if (g.materialId == m.id &&
                g.variant == v.label &&
                g.isRoll == (m.cost_model == "ROLL_AREA"))
            {
                if (g.isRoll)
                {
                    if (g.rollWidth == item.selectedRollWidth)
                    {
                        group = &g;
                        break;
                    }
                }
                else
                {
                    if (g.sheetWidth == item.selectedSheetFormat.width &&
                        g.sheetHeight == item.selectedSheetFormat.height)
                    {
                        group = &g;
                        break;
                    }
                }
            }
        }

        if (!group)
        {
            NestGroup newGroup;

            newGroup.materialId = m.id;
            newGroup.variant = v.label;

            newGroup.isRoll = (m.cost_model == "ROLL_AREA");

            newGroup.rollWidth = item.selectedRollWidth;

            newGroup.sheetWidth = item.selectedSheetFormat.width;
            newGroup.sheetHeight = item.selectedSheetFormat.height;

            groups.push_back(newGroup);

            group = &groups.back();
        }

        group->rects.push_back(
            {
                item.width,
                item.height,
                item.quantity
            });

        group->jobItemIndices.push_back(i);
    }

    return groups;
}

NestingJobResult NestingManager::calculate(
    const Job& job)
{
    NestingJobResult result;

    result.groups = buildGroups(job);

    NestingCore core;

    for (auto& group : result.groups)
    {
        if (group.isRoll)
        {
            // Roll groups will be handled next
            continue;
        }

        group.layouts =
            core.pack(
                group.rects,
                group.sheetWidth,
                group.sheetHeight);

        // -----------------------------
        // GROUP STATISTICS
        // -----------------------------
        group.sheetCount =
            static_cast<int>(group.layouts.size());

        double totalObjectArea = 0.0;

        for (const auto& r : group.rects)
        {
            totalObjectArea +=
                (r.width * r.height * r.quantity) / 1000000.0;
        }

        double sheetArea =
            (group.sheetWidth * group.sheetHeight) / 1000000.0;

        double totalSheetArea =
            sheetArea * group.sheetCount;

        if (totalSheetArea > 0.0)
        {
            group.wasteAreaM2 =
                totalSheetArea - totalObjectArea;

            group.efficiencyPercent =
                (totalObjectArea / totalSheetArea) * 100.0;
        }

        result.sheets.insert(
            result.sheets.end(),
            group.layouts.begin(),
            group.layouts.end());
    }

    return result;
}