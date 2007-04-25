require 'libmtag'

def test_file(fn)
	file = MTag::File.new(fn)
	print("%s - %s\n" % [file.tag.artist, file.tag.title])
	# file.tag.title = "foo";
	# file.save();
end

test_file(ARGV[0])
