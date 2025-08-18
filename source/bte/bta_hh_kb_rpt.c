
	for (xx = 0; xx < report_len; ++xx)
	{
		APPL_TRACE(DEBUG, "this_char = %02x", this_report[xx]);

		this_char = this_report[xx];

		if (this_char == 0x00)
			continue;

		if (this_char == BTA_HH_KB_CAPS_LOCK)
		{
			p_kb->caps_lock = p_kb->caps_lock ? FALSE : TRUE;
			continue;
		}

		if (0x04 <= this_char && this_char <= 0x1d)
		{
			this_char += 0x3d;
			goto _410;
		}

		if (0x1e <= this_char && this_char <= 0x26)
		{
			this_char += 0x13;
			goto _410;
		}

		if (0x3a <= this_char && this_char <= 0x45)
		{
			this_char += 0x36;
			goto _410;
		}

		if (0x27 <= this_char && this_char <= 0x38)
		{
			this_char = vir_key_tbl_1[this_char - 0x27];
			goto _410;
		}

		if (this_char == 0x5c)
		{
			this_char = 0xbc;
			goto _410;
		}

		if (0x49 > this_char)
			goto _3f0;
		if (this_char > 0x63)
			goto _3f0;
			this_char = vir_key_tbl_2[this_char - 0x49];
			if (this_char == 0xff)
			{
				p_kb->num_lock = p_kb->num_lock ? FALSE : TRUE;
				continue;
			}

			if (!p_kb->num_lock)
				goto _410;

			if (this_char == 0x62)
			{
				this_char = 0x30;
				goto _410;
			}

			if (this_char == 0x63)
			{
				this_char = 0xbe;
				goto _410;
			}

			this_char -= 0x28;
			goto _410;

		_3f0:
		{
			APPL_TRACE(ERROR,
			           "BTA_HhParseKeybdRpt:  Cannot interpret scan code \
	           0x%02x",
			           this_char);
			continue;
		}

		_410:
		p_data->caps_lock = p_kb->caps_lock;
		p_data->this_char = this_char;
	}
