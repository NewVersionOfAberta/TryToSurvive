#include "Anim_Directional.h"
#include "SpriteSheet.h"

void Anim_Directional::CropSprite() {
	sf::Vector2f padding = m_spriteSheet->GetSheetPadding();
	sf::Vector2f spacing = m_spriteSheet->GetSpriteSpacing();

	sf::IntRect rect((m_spriteSheet->GetSpriteSize().first * m_frameCurrent) /*+ padding.first + (spacing.first * m_frameCurrent)*/,
		(m_spriteSheet->GetSpriteSize().second * (m_frameRow + (short)m_spriteSheet->GetDirection()))
		/*+ padding.second + ((m_frameRow + (short)m_spriteSheet->GetDirection()) * spacing.second)*/,
		m_spriteSheet->GetSpriteSize().first, m_spriteSheet->GetSpriteSize().second);
	m_spriteSheet->CropSprite(rect);
}

void Anim_Directional::FrameStep() {
	bool b = SetFrame(m_frameCurrent + (m_frameStart <= m_frameEnd ? 1 : -1));
	if (b) { return; }
	if (m_loop) { SetFrame(m_frameStart); }
	else { SetFrame(m_frameEnd); Pause(); }
}

void Anim_Directional::ReadIn(std::stringstream& l_stream) {
	l_stream >> m_frameStart >> m_frameEnd >> m_frameRow
		>> m_frameTime >> m_frameActionStart >> m_frameActionEnd;
}