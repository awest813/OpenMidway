## 2026-03-21 - Adding Tooltips to Icon-Only Buttons
**Learning:** Icon-only buttons (like ICON_FA_XMARK or ICON_FA_ARROW_LEFT) in ImGui menus can be confusing for new users. Adding simple `ImGui::SetTooltip` on `ImGui::IsItemHovered()` is a low-risk, high-reward UX improvement.
**Action:** Always check for icon-only buttons in new ImGui views and add tooltips for clarity.
