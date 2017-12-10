CREATE OR REPLACE FUNCTION second_function (data character varying) RETURNS integer AS $$
BEGIN
    RAISE NOTICE 'HELLO WORLD';
END;
$$ LANGUAGE plpgsql;
