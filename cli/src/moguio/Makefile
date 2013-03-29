register: docs
	python setup.py sdist bdist_egg register upload

docs:
	cp README.md docs/
	cd docs && $(MAKE) all
	cd docs && $(MAKE) clean

clean:
	rm -rf moguio.egg-info
	rm -rf dist/
	rm -rf build/
	rm -rf *.pyc

install:
	sudo python setup.py install
