#include "Paint_Editor.h"

void ApplyPen(SDL_Texture* target, int x, int y, SDL_Renderer* renderer) {
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);

    int radius = globalEditor.brushSize;
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }

    SDL_SetRenderTarget(renderer, oldTarget);
}

void ApplyEraser(SDL_Texture* target, int x, int y, SDL_Renderer* renderer) {
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    int radius = globalEditor.brushSize;
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }

    SDL_SetRenderTarget(renderer, oldTarget);
}

void ApplyFill(SDL_Texture* target, SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
}

void DrawLineOnTexture(SDL_Texture* target, int x1, int y1, int x2, int y2, SDL_Renderer* renderer, bool isEraser) {
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    if (isEraser) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    } else {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
    }

    int radius = globalEditor.brushSize;

    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        if (radius <= 1) {
            SDL_RenderDrawPoint(renderer, x1, y1);
        } else {
            for (int w = -radius; w <= radius; w++) {
                for (int h = -radius; h <= radius; h++) {
                    if (w * w + h * h <= radius * radius) {
                        SDL_RenderDrawPoint(renderer, x1 + w, y1 + h);
                    }
                }
            }
        }

        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }

    SDL_SetRenderTarget(renderer, oldTarget);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void DrawCircleOnTexture(SDL_Texture* target, int centerX, int centerY, int radius, SDL_Renderer* renderer, bool fill) {
    if (!target || radius <= 0) return;
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);

    if (fill) {
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    } else {
        int x = radius - 1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);
        while (x >= y) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
            if (err <= 0) { y++; err += dy; dy += 2; }
            if (err > 0) { x--; dx += 2; err += dx - (radius << 1); }
        }
    }
    SDL_SetRenderTarget(renderer, nullptr);
}

void ClearCurrentDrawingLayer(SDL_Renderer* renderer) {
    if (selectedBackdropIndex < 0 || selectedBackdropIndex >= (int)projectBackdrops.size()) return;

    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    if (!target) return;

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, oldTarget);
}

void Draw_Image_Editor(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* currentTex, string itemName) {
    int barX = 110, barY = 95, barW = 920, barH = 110;
    SDL_Rect topBar = { barX, barY, barW, barH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &topBar);

    Drawtext(renderer, font, "Name:", barX + 20, barY + 20, {100, 100, 100, 255}, false);

    SDL_Rect nameBox = { barX + 80, barY + 15, 180, 35 };
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &nameBox);

    string displayName = itemName;
    if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
        displayName = projectBackdrops[selectedBackdropIndex].name;
    }
    Drawtext(renderer, font, displayName, nameBox.x + 10, nameBox.y + 5, {0, 0, 0, 255}, false);

    int colorX = barX + 300, colorY = barY + 32;
    SDL_Color colors[] = {{0, 0, 0, 255}, {255, 0, 0, 255}, {0, 0, 255, 255}};
    for (int i = 0; i < 3; i++) {
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
        for (int w = -15; w <= 15; w++) {
            for (int h = -15; h <= 15; h++) {
                if (w * w + h * h <= 225) SDL_RenderDrawPoint(renderer, colorX + i * 45 + w, colorY + h);
            }
        }
        if (globalEditor.currentColor.r == colors[i].r && globalEditor.currentColor.g == colors[i].g && globalEditor.currentColor.b == colors[i].b) {
            SDL_SetRenderDrawColor(renderer, 66, 133, 244, 255);
            SDL_Rect highlight = {colorX + i * 45 - 17, colorY - 17, 34, 34};
            SDL_RenderDrawRect(renderer, &highlight);
        }
    }

    int sizeX = barX + 450;
    int sizes[] = {2, 5, 10};
    for (int i = 0; i < 3; i++) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        int r = sizes[i] + 2;
        for (int w = -r; w <= r; w++) {
            for (int h = -r; h <= r; h++) {
                if (w * w + h * h <= r * r) SDL_RenderDrawPoint(renderer, sizeX + i * 40 + w, colorY + h);
            }
        }
        if (globalEditor.brushSize == sizes[i]) {
            SDL_SetRenderDrawColor(renderer, 66, 133, 244, 255);
            SDL_Rect highlight = {sizeX + i * 40 - 15, colorY - 15, 30, 30};
            SDL_RenderDrawRect(renderer, &highlight);
        }
    }

    int toolX = 115, toolY = 220, btnS = 45;
    for (int i = 0; i < 9; i++) {
        SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };

        bool isActive = false;
        if (i == 1) isActive = (globalEditor.activeTool == TOOL_PEN);
        else if (i == 2) isActive = (globalEditor.activeTool == TOOL_LINE);
        else if (i == 3) isActive = (globalEditor.activeTool == TOOL_CIRCLE);
        else if (i == 4) isActive = (globalEditor.activeTool == TOOL_TEXT);
        else if (i == 5) isActive = (globalEditor.activeTool == TOOL_FILL);
        else if (i == 6) isActive = (globalEditor.activeTool == TOOL_ERASER);

        SDL_SetRenderDrawColor(renderer, isActive ? 229 : 255, isActive ? 240 : 255, isActive ? 255 : 255, 255);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawColor(renderer, isActive ? 77 : 220, isActive ? 151 : 220, isActive ? 255 : 220, 255);
        SDL_RenderDrawRect(renderer, &r);

        if (i == 0) Drawtext(renderer, font, "C", r.x + 15, r.y + 12, {255, 0, 0, 255}, false);
        else if (i == 1) Drawtext(renderer, font, "P", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 2) Drawtext(renderer, font, "/", r.x + 18, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 3) Drawtext(renderer, font, "O", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 4) Drawtext(renderer, font, "T", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 5) Drawtext(renderer, font, "F", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 6) Drawtext(renderer, font, "E", r.x + 15, r.y + 12, {255, 0, 0, 255}, false);
        else if (i == 7) Drawtext(renderer, font, "S", r.x + 15, r.y + 12, {0, 150, 0, 255}, false);
        else if (i == 8) Drawtext(renderer, font, "<", r.x + 15, r.y + 12, {77, 151, 255, 255}, false);
    }

    SDL_Rect canvasBG = { 240, 220, 780, 520 };
    SDL_SetRenderDrawColor(renderer, 232, 237, 247, 255);
    SDL_RenderFillRect(renderer, &canvasBG);

    if (currentTex) {
        SDL_Rect imgPos = { 330, 280, 600, 380 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &imgPos);
        SDL_RenderCopy(renderer, currentTex, nullptr, &imgPos);

        if (selectedBackdropIndex >= 0 && projectBackdrops[selectedBackdropIndex].drawingLayer) {
            SDL_SetTextureBlendMode(projectBackdrops[selectedBackdropIndex].drawingLayer, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].drawingLayer, NULL, &imgPos);
        }

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &imgPos);
    }
}

