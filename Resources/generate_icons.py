import os
import math
from PIL import Image, ImageDraw

RESOURCES_DIR = r"d:\Projects\MyPlugins\Plugins\GameplayInventoryFramework\Resources"
ICONS_DIR = os.path.join(RESOURCES_DIR, "Icons")
os.makedirs(ICONS_DIR, exist_ok=True)

# Helper to scale 64-coord to 512-coord (factor of 8)
def S(val):
    return int(val * 8)

def save_icon(name, draw_fn, svg_content):
    # 1. Save SVG
    svg_path = os.path.join(ICONS_DIR, f"{name}.svg")
    with open(svg_path, "w", encoding="utf-8") as f:
        f.write(svg_content.strip())

    # 2. Render high-res 512x512 image with antialiased supersampling
    img = Image.new("RGBA", (512, 512), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw_fn(draw)

    # 3. Downsample to 64x64 and 16x16
    img64 = img.resize((64, 64), Image.Resampling.LANCZOS)
    img16 = img.resize((16, 16), Image.Resampling.LANCZOS)

    img64.save(os.path.join(ICONS_DIR, f"{name}_64.png"), "PNG")
    img16.save(os.path.join(ICONS_DIR, f"{name}_16.png"), "PNG")
    print(f"Generated icons for: {name}")

# ========================================================
# 1. InventoryComponent (Backpack with pockets & handle)
# ========================================================
def draw_InventoryComponent(draw):
    WHITE = (255, 255, 255, 255)
    # Main body
    draw.rounded_rectangle([S(14), S(18), S(50), S(52)], radius=S(6), outline=WHITE, width=S(3))
    # Top handle
    draw.arc([S(22), S(8), S(42), S(24)], start=180, end=0, fill=WHITE, width=S(3))
    # Top flap
    draw.line([S(14), S(26), S(50), S(26)], fill=WHITE, width=S(3))
    # Front pocket
    draw.rounded_rectangle([S(20), S(32), S(44), S(46)], radius=S(3), outline=WHITE, width=S(2.5))
    # Pocket buckle
    draw.rectangle([S(30), S(24), S(34), S(34)], fill=WHITE)

svg_InventoryComponent = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <path d="M 22,16 A 10,10 0 0,1 42,16" fill="none" stroke="white" stroke-width="3" stroke-linecap="round" />
  <rect x="14" y="18" width="36" height="34" rx="6" fill="none" stroke="white" stroke-width="3" />
  <line x1="14" y1="26" x2="50" y2="26" stroke="white" stroke-width="3" />
  <rect x="20" y="32" width="24" height="14" rx="3" fill="none" stroke="white" stroke-width="2.5" />
  <rect x="30" y="24" width="4" height="10" fill="white" />
</svg>"""

save_icon("InventoryComponent", draw_InventoryComponent, svg_InventoryComponent)

# ========================================================
# 2. InventoryManagerComponent (Dual containers with transfer arrows)
# ========================================================
def draw_InventoryManagerComponent(draw):
    WHITE = (255, 255, 255, 255)
    # Left container
    draw.rounded_rectangle([S(8), S(22), S(24), S(44)], radius=S(3), outline=WHITE, width=S(2.5))
    draw.line([S(8), S(29), S(24), S(29)], fill=WHITE, width=S(2))
    # Right container
    draw.rounded_rectangle([S(40), S(22), S(56), S(44)], radius=S(3), outline=WHITE, width=S(2.5))
    draw.line([S(40), S(29), S(56), S(29)], fill=WHITE, width=S(2))
    # Top transfer arrow (left to right)
    draw.line([S(26), S(16), S(38), S(16)], fill=WHITE, width=S(3))
    draw.line([S(33), S(11), S(38), S(16)], fill=WHITE, width=S(3))
    draw.line([S(33), S(21), S(38), S(16)], fill=WHITE, width=S(3))
    # Bottom transfer arrow (right to left)
    draw.line([S(38), S(50), S(26), S(50)], fill=WHITE, width=S(3))
    draw.line([S(31), S(45), S(26), S(50)], fill=WHITE, width=S(3))
    draw.line([S(31), S(55), S(26), S(50)], fill=WHITE, width=S(3))

svg_InventoryManagerComponent = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <rect x="8" y="22" width="16" height="22" rx="3" fill="none" stroke="white" stroke-width="2.5" />
  <line x1="8" y1="29" x2="24" y2="29" stroke="white" stroke-width="2" />
  <rect x="40" y="22" width="16" height="22" rx="3" fill="none" stroke="white" stroke-width="2.5" />
  <line x1="40" y1="29" x2="56" y2="29" stroke="white" stroke-width="2" />
  <path d="M 26,16 L 38,16 M 33,11 L 38,16 L 33,21" fill="none" stroke="white" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" />
  <path d="M 38,50 L 26,50 M 31,45 L 26,50 L 31,55" fill="none" stroke="white" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" />
</svg>"""

save_icon("InventoryManagerComponent", draw_InventoryManagerComponent, svg_InventoryManagerComponent)

# ========================================================
# 3. ItemComponent (Pedestal with floating loot gem)
# ========================================================
def draw_ItemComponent(draw):
    WHITE = (255, 255, 255, 255)
    # Pedestal base
    draw.line([S(14), S(52), S(50), S(52)], fill=WHITE, width=S(3))
    draw.polygon([(S(22), S(52)), (S(26), S(46)), (S(38), S(46)), (S(42), S(52))], outline=WHITE, width=S(2.5))
    # Floating loot diamond
    draw.polygon([(S(32), S(10)), (S(46), S(26)), (S(32), S(42)), (S(18), S(26))], outline=WHITE, width=S(3))
    # Center sparkle / facet
    draw.polygon([(S(32), S(18)), (S(40), S(26)), (S(32), S(34)), (S(24), S(26))], fill=WHITE)

svg_ItemComponent = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <line x1="14" y1="52" x2="50" y2="52" stroke="white" stroke-width="3" stroke-linecap="round" />
  <polygon points="22,52 26,46 38,46 42,52" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <polygon points="32,10 46,26 32,42 18,26" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <polygon points="32,18 40,26 32,34 24,26" fill="white" />
</svg>"""

save_icon("ItemComponent", draw_ItemComponent, svg_ItemComponent)

# ========================================================
# 4. InventoryItemDefinition (DataAsset scroll with diamond)
# ========================================================
def draw_InventoryItemDefinition(draw):
    WHITE = (255, 255, 255, 255)
    # Document outline with folded top-right corner
    pts = [(S(16), S(10)), (S(38), S(10)), (S(48), S(20)), (S(48), S(54)), (S(16), S(54))]
    draw.polygon(pts, outline=WHITE, width=S(3))
    # Folded corner crease
    draw.line([S(38), S(10), S(38), S(20)], fill=WHITE, width=S(2.5))
    draw.line([S(38), S(20), S(48), S(20)], fill=WHITE, width=S(2.5))
    # Central item diamond
    draw.polygon([(S(32), S(27)), (S(39), S(34)), (S(32), S(41)), (S(25), S(34))], fill=WHITE)
    # Footer data line
    draw.line([S(22), S(47), S(42), S(47)], fill=WHITE, width=S(2.5))

svg_InventoryItemDefinition = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="16,10 38,10 48,20 48,54 16,54" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <path d="M 38,10 L 38,20 L 48,20" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <polygon points="32,27 39,34 32,41 25,34" fill="white" />
  <line x1="22" y1="47" x2="42" y2="47" stroke="white" stroke-width="2.5" stroke-linecap="round" />
</svg>"""

save_icon("InventoryItemDefinition", draw_InventoryItemDefinition, svg_InventoryItemDefinition)

# ========================================================
# 5. InventoryItemInstance (Dynamic instance diamond with star ticks)
# ========================================================
def draw_InventoryItemInstance(draw):
    WHITE = (255, 255, 255, 255)
    # Main outer diamond
    draw.polygon([(S(32), S(8)), (S(54), S(32)), (S(32), S(56)), (S(10), S(32))], outline=WHITE, width=S(3))
    # Center circle
    draw.ellipse([S(25), S(25), S(39), S(39)], fill=WHITE)
    # Cardinal ticks
    draw.line([S(32), S(13), S(32), S(19)], fill=WHITE, width=S(3))
    draw.line([S(32), S(45), S(32), S(51)], fill=WHITE, width=S(3))
    draw.line([S(15), S(32), S(21), S(32)], fill=WHITE, width=S(3))
    draw.line([S(43), S(32), S(49), S(32)], fill=WHITE, width=S(3))

svg_InventoryItemInstance = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="32,8 54,32 32,56 10,32" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <circle cx="32" cy="32" r="7" fill="white" />
  <line x1="32" y1="13" x2="32" y2="19" stroke="white" stroke-width="3" stroke-linecap="round" />
  <line x1="32" y1="45" x2="32" y2="51" stroke="white" stroke-width="3" stroke-linecap="round" />
  <line x1="15" y1="32" x2="21" y2="32" stroke="white" stroke-width="3" stroke-linecap="round" />
  <line x1="43" y1="32" x2="49" y2="32" stroke="white" stroke-width="3" stroke-linecap="round" />
</svg>"""

save_icon("InventoryItemInstance", draw_InventoryItemInstance, svg_InventoryItemInstance)

# ========================================================
# 6. InventoryItemFragment (Interlocking puzzle badge)
# ========================================================
def draw_InventoryItemFragment(draw):
    WHITE = (255, 255, 255, 255)
    # Puzzle piece outline
    pts = [
        (S(16), S(16)), (S(26), S(16)), (S(26), S(11)), (S(38), S(11)), (S(38), S(16)),
        (S(48), S(16)), (S(48), S(26)), (S(53), S(26)), (S(53), S(38)), (S(48), S(38)),
        (S(48), S(48)), (S(38), S(48)), (S(38), S(43)), (S(26), S(43)), (S(26), S(48)),
        (S(16), S(48)), (S(16), S(38)), (S(21), S(38)), (S(21), S(26)), (S(16), S(26))
    ]
    draw.polygon(pts, outline=WHITE, width=S(3))
    draw.ellipse([S(29), S(29), S(35), S(35)], fill=WHITE)

svg_InventoryItemFragment = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="16,16 26,16 26,11 38,11 38,16 48,16 48,26 53,26 53,38 48,38 48,48 38,48 38,43 26,43 26,48 16,48 16,38 21,38 21,26 16,26" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <circle cx="32" cy="32" r="3" fill="white" />
</svg>"""

save_icon("InventoryItemFragment", draw_InventoryItemFragment, svg_InventoryItemFragment)

# ========================================================
# 7. Fragment_UIInfo (UI Card with lines)
# ========================================================
def draw_Fragment_UIInfo(draw):
    WHITE = (255, 255, 255, 255)
    draw.rounded_rectangle([S(12), S(14), S(52), S(50)], radius=S(4), outline=WHITE, width=S(3))
    draw.line([S(12), S(24), S(52), S(24)], fill=WHITE, width=S(2.5))
    draw.line([S(18), S(32), S(36), S(32)], fill=WHITE, width=S(2.5))
    draw.line([S(18), S(40), S(44), S(40)], fill=WHITE, width=S(2.5))
    draw.ellipse([S(43), S(18), S(47), S(22)], fill=WHITE)

svg_Fragment_UIInfo = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <rect x="12" y="14" width="40" height="36" rx="4" fill="none" stroke="white" stroke-width="3" />
  <line x1="12" y1="24" x2="52" y2="24" stroke="white" stroke-width="2.5" />
  <line x1="18" y1="32" x2="36" y2="32" stroke="white" stroke-width="2.5" stroke-linecap="round" />
  <line x1="18" y1="40" x2="44" y2="40" stroke="white" stroke-width="2.5" stroke-linecap="round" />
  <circle cx="45" cy="19" r="2" fill="white" />
</svg>"""

save_icon("Fragment_UIInfo", draw_Fragment_UIInfo, svg_Fragment_UIInfo)

# ========================================================
# 8. Fragment_ItemStats (Shield with stat bars)
# ========================================================
def draw_Fragment_ItemStats(draw):
    WHITE = (255, 255, 255, 255)
    # Shield
    pts = [(S(16), S(14)), (S(48), S(14)), (S(48), S(32)), (S(32), S(52)), (S(16), S(32))]
    draw.polygon(pts, outline=WHITE, width=S(3))
    # Stat bars
    draw.rectangle([S(22), S(34), S(26), S(42)], fill=WHITE)
    draw.rectangle([S(30), S(28), S(34), S(42)], fill=WHITE)
    draw.rectangle([S(38), S(22), S(42), S(42)], fill=WHITE)

svg_Fragment_ItemStats = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="16,14 48,14 48,32 32,52 16,32" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <rect x="22" y="34" width="4" height="8" fill="white" />
  <rect x="30" y="28" width="4" height="14" fill="white" />
  <rect x="38" y="22" width="4" height="20" fill="white" />
</svg>"""

save_icon("Fragment_ItemStats", draw_Fragment_ItemStats, svg_Fragment_ItemStats)

# ========================================================
# 9. Fragment_Stacking (Stacked coins / layers)
# ========================================================
def draw_Fragment_Stacking(draw):
    WHITE = (255, 255, 255, 255)
    # Top ellipse
    draw.ellipse([S(14), S(12), S(50), S(26)], outline=WHITE, width=S(2.5))
    # Middle rim
    draw.arc([S(14), S(22), S(50), S(36)], start=0, end=180, fill=WHITE, width=S(2.5))
    # Bottom rim
    draw.arc([S(14), S(32), S(50), S(46)], start=0, end=180, fill=WHITE, width=S(2.5))
    # Side lines
    draw.line([S(14), S(19), S(14), S(39)], fill=WHITE, width=S(2.5))
    draw.line([S(50), S(19), S(50), S(39)], fill=WHITE, width=S(2.5))
    # Plus sign
    draw.line([S(32), S(25), S(32), S(33)], fill=WHITE, width=S(2.5))
    draw.line([S(28), S(29), S(36), S(29)], fill=WHITE, width=S(2.5))

svg_Fragment_Stacking = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <ellipse cx="32" cy="19" rx="18" ry="7" fill="none" stroke="white" stroke-width="2.5" />
  <path d="M 14,19 L 14,39 A 18,7 0 0,0 50,39 L 50,19" fill="none" stroke="white" stroke-width="2.5" />
  <path d="M 14,29 A 18,7 0 0,0 50,29" fill="none" stroke="white" stroke-width="2.5" />
  <path d="M 32,25 L 32,33 M 28,29 L 36,29" stroke="white" stroke-width="2.5" stroke-linecap="round" />
</svg>"""

save_icon("Fragment_Stacking", draw_Fragment_Stacking, svg_Fragment_Stacking)

# ========================================================
# 10. Fragment_Montage (Clapperboard / film frame)
# ========================================================
def draw_Fragment_Montage(draw):
    WHITE = (255, 255, 255, 255)
    # Clapper body
    draw.rounded_rectangle([S(14), S(26), S(50), S(50)], radius=S(2), outline=WHITE, width=S(3))
    # Slanted clapper bar
    draw.polygon([(S(12), S(24)), (S(48), S(14)), (S(52), S(22)), (S(16), S(32))], outline=WHITE, width=S(2.5))
    # Play triangle
    draw.polygon([(S(28), S(33)), (S(40), S(38)), (S(28), S(43))], fill=WHITE)

svg_Fragment_Montage = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <rect x="14" y="26" width="36" height="24" rx="2" fill="none" stroke="white" stroke-width="3" />
  <polygon points="12,24 48,14 52,22 16,32" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <polygon points="28,33 40,38 28,43" fill="white" />
</svg>"""

save_icon("Fragment_Montage", draw_Fragment_Montage, svg_Fragment_Montage)

# ========================================================
# 11. Fragment_Attachment (Mounting clamp / rail)
# ========================================================
def draw_Fragment_Attachment(draw):
    WHITE = (255, 255, 255, 255)
    pts = [
        (S(14), S(24)), (S(24), S(24)), (S(24), S(14)), (S(40), S(14)), (S(40), S(24)),
        (S(50), S(24)), (S(50), S(34)), (S(40), S(34)), (S(40), S(50)), (S(24), S(50)),
        (S(24), S(34)), (S(14), S(34))
    ]
    draw.polygon(pts, outline=WHITE, width=S(3))
    draw.ellipse([S(28), S(28), S(36), S(36)], fill=WHITE)

svg_Fragment_Attachment = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="14,24 24,24 24,14 40,14 40,24 50,24 50,34 40,34 40,50 24,50 24,34 14,34" fill="none" stroke="white" stroke-width="3" stroke-linejoin="round" />
  <circle cx="32" cy="32" r="4" fill="white" />
</svg>"""

save_icon("Fragment_Attachment", draw_Fragment_Attachment, svg_Fragment_Attachment)

# ========================================================
# 12. Fragment_Audio (Speaker with acoustic arcs)
# ========================================================
def draw_Fragment_Audio(draw):
    WHITE = (255, 255, 255, 255)
    # Speaker horn
    draw.polygon([(S(12), S(25)), (S(22), S(25)), (S(32), S(15)), (S(32), S(49)), (S(22), S(39)), (S(12), S(39))], fill=WHITE)
    # Sound arcs
    draw.arc([S(28), S(24), S(44), S(40)], start=-60, end=60, fill=WHITE, width=S(3))
    draw.arc([S(26), S(16), S(52), S(48)], start=-60, end=60, fill=WHITE, width=S(3))

svg_Fragment_Audio = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="12,25 22,25 32,15 32,49 22,39 12,39" fill="white" />
  <path d="M 38,24 A 10,10 0 0,1 38,40" fill="none" stroke="white" stroke-width="3" stroke-linecap="round" />
  <path d="M 45,17 A 18,18 0 0,1 45,47" fill="none" stroke="white" stroke-width="3" stroke-linecap="round" />
</svg>"""

save_icon("Fragment_Audio", draw_Fragment_Audio, svg_Fragment_Audio)

# ========================================================
# 13. Fragment_VFX (Particle burst / magic star)
# ========================================================
def draw_Fragment_VFX(draw):
    WHITE = (255, 255, 255, 255)
    # 4-point radiant star
    pts = [
        (S(32), S(10)), (S(35), S(27)), (S(54), S(32)), (S(35), S(37)),
        (S(32), S(54)), (S(29), S(37)), (S(10), S(32)), (S(29), S(27))
    ]
    draw.polygon(pts, fill=WHITE)
    # Sparkle dots
    draw.ellipse([S(17), S(17), S(21), S(21)], fill=WHITE)
    draw.ellipse([S(43), S(17), S(47), S(21)], fill=WHITE)
    draw.ellipse([S(17), S(43), S(21), S(47)], fill=WHITE)
    draw.ellipse([S(43), S(43), S(47), S(47)], fill=WHITE)

svg_Fragment_VFX = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="32,10 35,27 54,32 35,37 32,54 29,37 10,32 29,27" fill="white" />
  <circle cx="19" cy="19" r="2.5" fill="white" />
  <circle cx="45" cy="19" r="2.5" fill="white" />
  <circle cx="19" cy="45" r="2.5" fill="white" />
  <circle cx="45" cy="45" r="2.5" fill="white" />
</svg>"""

save_icon("Fragment_VFX", draw_Fragment_VFX, svg_Fragment_VFX)

# ========================================================
# 14. Fragment_Mesh (3D wireframe mesh cube)
# ========================================================
def draw_Fragment_Mesh(draw):
    WHITE = (255, 255, 255, 255)
    # Top face
    draw.polygon([(S(32), S(12)), (S(50), S(22)), (S(32), S(32)), (S(14), S(22))], outline=WHITE, width=S(2.5))
    # Left face
    draw.polygon([(S(14), S(22)), (S(32), S(32)), (S(32), S(52)), (S(14), S(42))], outline=WHITE, width=S(2.5))
    # Right face
    draw.polygon([(S(32), S(32)), (S(50), S(22)), (S(50), S(42)), (S(32), S(52))], outline=WHITE, width=S(2.5))
    # Center vertex dot
    draw.ellipse([S(30), S(30), S(34), S(34)], fill=WHITE)

svg_Fragment_Mesh = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <polygon points="32,12 50,22 32,32 14,22" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <polygon points="14,22 32,32 32,52 14,42" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <polygon points="32,32 50,22 50,42 32,52" fill="none" stroke="white" stroke-width="2.5" stroke-linejoin="round" />
  <circle cx="32" cy="32" r="2.5" fill="white" />
</svg>"""

save_icon("Fragment_Mesh", draw_Fragment_Mesh, svg_Fragment_Mesh)

# ========================================================
# 15. Fragment_Material (Shading material sphere)
# ========================================================
def draw_Fragment_Material(draw):
    WHITE = (255, 255, 255, 255)
    # Outer circle
    draw.ellipse([S(12), S(12), S(52), S(52)], outline=WHITE, width=S(3))
    # Inner meridian ellipse
    draw.ellipse([S(22), S(12), S(42), S(52)], outline=WHITE, width=S(2))
    # Equator line
    draw.line([S(12), S(32), S(52), S(32)], fill=WHITE, width=S(2))
    # Specular highlight
    draw.ellipse([S(22), S(20), S(28), S(26)], fill=WHITE)

svg_Fragment_Material = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <circle cx="32" cy="32" r="20" fill="none" stroke="white" stroke-width="3" />
  <ellipse cx="32" cy="32" rx="10" ry="20" fill="none" stroke="white" stroke-width="2" />
  <line x1="12" y1="32" x2="52" y2="32" stroke="white" stroke-width="2" />
  <circle cx="25" cy="23" r="3.5" fill="white" />
</svg>"""

save_icon("Fragment_Material", draw_Fragment_Material, svg_Fragment_Material)

# ========================================================
# 16. Fragment_Texture (Picture canvas / portrait)
# ========================================================
def draw_Fragment_Texture(draw):
    WHITE = (255, 255, 255, 255)
    # Frame
    draw.rounded_rectangle([S(12), S(14), S(52), S(50)], radius=S(3), outline=WHITE, width=S(3))
    # Sun
    draw.ellipse([S(20), S(21), S(26), S(27)], fill=WHITE)
    # Mountain peaks
    pts = [(S(14), S(46)), (S(26), S(32)), (S(35), S(42)), (S(41), S(35)), (S(50), S(46))]
    draw.polygon(pts, fill=WHITE)

svg_Fragment_Texture = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <rect x="12" y="14" width="40" height="36" rx="3" fill="none" stroke="white" stroke-width="3" />
  <circle cx="23" cy="24" r="3.5" fill="white" />
  <polygon points="14,46 26,32 35,42 41,35 50,46" fill="white" />
</svg>"""

save_icon("Fragment_Texture", draw_Fragment_Texture, svg_Fragment_Texture)

# ========================================================
# 17. Plugin Icon (Icon128.png)
# ========================================================
def generate_plugin_icon():
    # 512x512 downsampled to 128x128
    img = Image.new("RGBA", (512, 512), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Dark rounded background tile
    draw.rounded_rectangle([32, 32, 480, 480], radius=80, fill=(28, 33, 40, 255), outline=(72, 90, 115, 255), width=16)

    # Gradient-style inner border
    draw.rounded_rectangle([48, 48, 464, 464], radius=64, outline=(45, 120, 190, 200), width=6)

    # Central Backpack Icon (scaled to 128 canvas)
    WHITE = (245, 248, 255, 255)
    CYAN = (80, 200, 255, 255)

    # Backpack handle
    draw.arc([200, 100, 312, 220], start=180, end=0, fill=WHITE, width=22)

    # Main backpack body
    draw.rounded_rectangle([130, 170, 382, 430], radius=50, outline=WHITE, width=22)

    # Backpack top flap
    draw.polygon([(130, 240), (256, 290), (382, 240), (382, 170), (130, 170)], fill=(38, 45, 58, 255), outline=WHITE, width=16)

    # Front pouch
    draw.rounded_rectangle([180, 310, 332, 400], radius=24, fill=(38, 45, 58, 255), outline=WHITE, width=16)

    # Pouch lock / glowing cyan gem
    draw.polygon([(256, 260), (276, 290), (256, 320), (236, 290)], fill=CYAN)

    img128 = img.resize((128, 128), Image.Resampling.LANCZOS)
    img128.save(os.path.join(RESOURCES_DIR, "Icon128.png"), "PNG")
    print("Generated Icon128.png successfully!")

generate_plugin_icon()
print("All icons successfully generated!")
