require 'mtag'

def test_file(fn)
  file = MTag::File.new(fn)
  file.strip("id3v1")
  p file.tag.get_all
  print("%s - %s\n" % [file.tag.artist, file.tag.title])
  file.tag.set("artist", "foo")
  print("%s - %s\n" % [file.tag.get("artist"), file.tag.get("title")])
  print("%s - %s\n" % [file.tag.artist, file.tag.title])
  # file.tag.title = "foo";
  # file.save();
end

test_file(ARGV[0])
