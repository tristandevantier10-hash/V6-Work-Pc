#include "SheetPreviewPanel.h"
#include "NestingRender.h"
#include "Renderer.h"
#include "iostream"
#include <algorithm>

SheetPreviewPanel::SheetPreviewPanel()
{
    setPosition(350, 120);

    setSize(800, 620);
}


void SheetPreviewPanel::setSheets(
    const std::vector<Sheet>& newSheets)
{
    sheets = newSheets;
}

void SheetPreviewPanel::render(Renderer& renderer)
{
    SDL_Renderer* sdl =
        renderer.getSDLRenderer();

    SDL_Rect panel =
    {
        getX(),
        getY(),
        getWidth(),
        getHeight()
    };

    renderer.fillRect(
        panel,
        SDL_Color{ 25, 25, 30, 255 });

    renderer.pushClip(panel);

    float scale = calculateScale();

    int y = panel.y + 20 - scrollY;

    int sheetNumber = 1;


    for (const auto& sheet : sheets)
    {
        float drawnWidth =
            sheet.width * scale;

        int x =
            panel.x +
            (panel.w - static_cast<int>(drawnWidth)) / 2;

        std::string title =
            "Sheet " + std::to_string(sheetNumber) +
            "  (" +
            std::to_string((int)sheet.width) +
            " x " +
            std::to_string((int)sheet.height) +
            " mm)";

        int textWidth =
            renderer.getTextWidth(title);

        int titleX =
            x +
            (static_cast<int>(drawnWidth) - textWidth) / 2;

        renderer.drawText(
            title,
            titleX,
            y);

        y += 30;

        NestingRenderer::drawSheet(
            renderer,
            sheet,
            x,
            y,
            scale);

        y += static_cast<int>(sheet.height * scale + 80);

        sheetNumber++;
    }

    renderer.popClip();
}

void SheetPreviewPanel::update(const SDL_Event& e)
{
    if (e.type == SDL_MOUSEWHEEL)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        SDL_Point mouse{ mx, my };

        if (!SDL_PointInRect(&mouse, &bounds))
            return;

        scrollY -= e.wheel.y * 40;

        float contentHeight =
            calculateContentHeight();

        maxScrollY =
            contentHeight - getHeight();

        if (maxScrollY < 0)
            maxScrollY = 0;

        if (scrollY < 0)
            scrollY = 0;

        if (scrollY > maxScrollY)
            scrollY = maxScrollY;
    }
}

float SheetPreviewPanel::calculateScale() const
{
    if (sheets.empty())
        return 1.0f;


    double maxWidth = 0;


    for (const auto& sheet : sheets)
    {
        if (sheet.width > maxWidth)
            maxWidth = sheet.width;
    }


    float availableWidth =
        getWidth() - 40;


    float scaleX =
        availableWidth / maxWidth;


    return scaleX;
}

float SheetPreviewPanel::calculateContentHeight() const
{
    float scale = calculateScale();

    float height = 20; // top padding


    for (const auto& sheet : sheets)
    {
        height += 30; // title

        height +=
            (sheet.height * scale);

        height += 80; // spacing
    }


    return height + 20;
}