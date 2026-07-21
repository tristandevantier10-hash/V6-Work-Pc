#pragma once

#include <vector>
#include "Panel.h"
#include "NestingEngine.h"

class Renderer;

class SheetPreviewPanel : public Panel
{
public:

    SheetPreviewPanel();

    void setSheets(
        const std::vector<Sheet>& sheets);

    void render(Renderer& renderer) override;

    void update(const SDL_Event& e) override;

private:

    std::vector<Sheet> sheets;

    float scrollY = 0;

    float calculateScale() const;

    float calculateContentHeight() const;

    float maxScrollY = 0;
};
