/*
 * main.c
 *
 * Created: 05/03/2019 18:00:58
 *  Author: eduardo
 */ 

#include <asf.h>
#include "tfont.h"
#include "sourcecodepro_28.h"
#include "calibri_36.h"
#include "arial_72.h"

#define YEAR        2018
#define MONTH      3
#define DAY         19
#define WEEK        12
#define HOUR        15
#define MINUTE      45
#define SECOND      0

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        16
#define BUT1_PIO_IDX       28u
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO           PIOC
#define BUT2_PIO_ID        12
#define BUT2_PIO_IDX       31u
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO           PIOA
#define BUT3_PIO_ID        10
#define BUT3_PIO_IDX       19u
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

volatile float radius = 0.325;
volatile int counter = 0;
volatile int but_flag = 0;
volatile int dtc = 0;

volatile Bool f_rtt_alarme = false;


struct ili9488_opt_t g_ili9488_display_opt;

void configure_lcd(void){
	/* Initialize display parameter */
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	/* Initialize LCD */
	ili9488_init(&g_ili9488_display_opt);
	ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
	
}


void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
	char *p = text;
	while(*p != NULL) {
		char letter = *p;
		int letter_offset = letter - font->start_char;
		if(letter <= font->end_char) {
			tChar *current_char = font->chars + letter_offset;
			ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
			x += current_char->image->width + spacing;
		}
		p++;
	}	
}

void BUT1_Handler(){
	
}

void BUT2_Handler(){
	
}

void BUT3_Handler(){
	but_flag = 0;
	counter += 1;
	dtc += counter;
}

void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {  }

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {// BLINK Led
		f_rtt_alarme = true;                  // flag RTT alarme
	}
}

void But_init(){
	
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	pio_set_input(BUT1_PIO,BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_input(BUT2_PIO,BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_input(BUT3_PIO,BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	
	pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
	
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_IT_RISE_EDGE, BUT1_Handler);
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_IT_RISE_EDGE, BUT2_Handler);
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_IT_RISE_EDGE, BUT3_Handler);
	
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_EnableIRQ(BUT3_PIO_ID);
		
	NVIC_SetPriority(BUT1_PIO_ID, 1);
	NVIC_SetPriority(BUT2_PIO_ID, 1);
	NVIC_SetPriority(BUT3_PIO_ID, 1);
}

void RTC_init(){
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(RTC, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(RTC, YEAR, MONTH, DAY, WEEK);
	rtc_set_time(RTC, HOUR, MINUTE, SECOND);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 0);
	NVIC_EnableIRQ(RTC_IRQn);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(RTC,  RTC_IER_ALREN);

}

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));
	
	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);
}


int main(void) {
	board_init();
	sysclk_init();	
	configure_lcd();
	But_init();
	RTC_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	char speed_buffer[32];
	char rotation_counter_buffer[8];
	char distance_buffer[32];
	
	float distance = 0;
	float speed = 0;
	float w;
	
	font_draw_text(&calibri_36, "BIKE", 120, 50, 1);
	font_draw_text(&calibri_36, "VELOCIDADE", 50, 100, 1);
	sprintf(speed_buffer, "%f", speed);
	font_draw_text(&calibri_36, speed_buffer, 50, 150, 1);
	font_draw_text(&calibri_36, "DISTANCIA", 50, 200, 1);
	sprintf(distance_buffer, "%f", distance);
	font_draw_text(&calibri_36, distance_buffer, 50, 250, 1);
	font_draw_text(&calibri_36, "TEMPO", 50, 300, 1);
	
	sprintf(rotation_counter_buffer, "%d", counter);
	font_draw_text(&calibri_36, rotation_counter_buffer, 50, 400, 1);
	
	f_rtt_alarme = true;
	while(1) {
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		if(but_flag == 0){
			sprintf(rotation_counter_buffer, "%d", counter);
			font_draw_text(&calibri_36, rotation_counter_buffer, 50, 400, 1);
		}
		if (f_rtt_alarme){
			uint16_t pllPreScale = (int) (((float) 32768) / 1);
			uint32_t irqRTTvalue  = 4;
			RTT_init(pllPreScale, irqRTTvalue);
			
			distance = 2*3.14*radius*counter;
			
			w = 2*3.14*dtc/4;
			dtc = 0;
			speed = w*radius/3.6;
			
			sprintf(speed_buffer, "%f", speed);
			font_draw_text(&calibri_36, speed_buffer, 50, 150, 1);         
      	
			sprintf(distance_buffer, "%f", distance);
			font_draw_text(&calibri_36, distance_buffer, 50, 250, 1);
			f_rtt_alarme = false;
		}
	}
}